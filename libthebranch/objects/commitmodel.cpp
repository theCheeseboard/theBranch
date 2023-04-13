#include "commitmodel.h"

#include "commit.h"
#include "libgit/lgcommit.h"
#include "libgit/lgreference.h"
#include "libgit/lgrepository.h"
#include "libgit/lgrevwalk.h"
#include "reference.h"
#include "repository.h"
#include <QPainter>
#include <git2.h>

struct CommitModelPrivate {
        RepositoryPtr repo = nullptr;
        QList<CommitPtr> commits;

        QList<QList<int>> graphColumns;
        QList<QList<int>> parentGraphColumns;
        QList<QList<int>> passthroughGraphColumns;

        CommitPtr startPoint;
};

CommitModel::CommitModel(QObject* parent) :
    QAbstractListModel(parent) {
    d = new CommitModelPrivate();
}

CommitModel::~CommitModel() {
    delete d;
}

void CommitModel::setStartPoint(CommitPtr startPoint) {
    d->startPoint = startPoint;
    reloadData();
}

void CommitModel::setRepository(RepositoryPtr repo) {
    d->repo = repo;
    connect(repo.data(), &Repository::repositoryUpdated, this, &CommitModel::reloadData);
    reloadData();
}

void CommitModel::reloadData() {
    if (!d->repo) return;
    if (!d->repo->git_repository()) return;

    CommitPtr startPoint = d->repo->head()->asCommit();
    if (d->startPoint) startPoint = d->startPoint;

    d->commits = startPoint->history();

    d->graphColumns.clear();
    d->parentGraphColumns.clear();
    d->passthroughGraphColumns.clear();
    if (!d->commits.isEmpty()) {
        QMap<int, CommitPtr> parents;
        parents.insert(0, d->commits.first());
        for (auto commit : d->commits) {
            QList<int> myParentCols;
            for (auto parentCommit : parents.values()) {
                if (parentCommit->equal(commit)) myParentCols.append(parents.key(parentCommit));
            }

            int firstParentCol = myParentCols.first();
            for (auto parent : myParentCols) {
                parents.remove(parent);
                firstParentCol = qMin(firstParentCol, parent);
            }
            d->graphColumns.append(myParentCols);

            d->passthroughGraphColumns.append(parents.keys());

            QList<int> childrenCols;
            for (const auto& child : commit->parents()) {
                int i = firstParentCol;
                while (parents.contains(i)) i++;
                parents.insert(i, child);
                childrenCols.append(i);
            }
            d->parentGraphColumns.append(childrenCols);
        }
    }

    emit dataChanged(index(0), index(rowCount()));
}

int CommitModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    if (!d->repo) return 0;

    return d->commits.count();
}

QVariant CommitModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) return QVariant();
    if (!d->repo) return 0;

    CommitPtr commit = d->commits.at(index.row());
    switch (role) {
        case Qt::DisplayRole:
        case CommitMessage:
            return commit->commitMessage();
        case CommitHash:
            return commit->commitHash();
        case ShortCommitHash:
            return commit->shortCommitHash();
        case AuthorName:
            return commit->authorName();
        case Commit:
            return QVariant::fromValue(commit);
        case GraphColumn:
            return QVariant::fromValue(d->graphColumns.at(index.row()));
        case ParentGraphColumns:
            return QVariant::fromValue(d->parentGraphColumns.at(index.row()));
        case PassthroughGraphColumns:
            return QVariant::fromValue(d->passthroughGraphColumns.at(index.row()));
        case CommitDate:
            return commit->date();
    }

    return QVariant();
}

CommitDelegate::CommitDelegate(QObject* parent) :
    QStyledItemDelegate(parent) {
}

void CommitDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    painter->save();
    option.widget->style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &option, painter, option.widget);
    painter->restore();

    painter->save();
    paintCalculator(option, index, painter).performPaint();
    painter->restore();
}

QSize CommitDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
    return paintCalculator(option, index).sizeWithMargins().toSize();
}

tPaintCalculator CommitDelegate::paintCalculator(const QStyleOptionViewItem& option, const QModelIndex& index, QPainter* painter) const {
    tPaintCalculator calculator;
    calculator.setPainter(painter);
    calculator.setLayoutDirection(option.direction);
    calculator.setDrawBounds(option.rect);

    QFont lowerFont = option.font;
    lowerFont.setPointSize(lowerFont.pointSizeF() * 0.8);
    QFontMetrics lowerFontMetrics(lowerFont);

    // Build the commit graph
    auto fromCols = index.data(CommitModel::GraphColumn).value<QList<int>>();
    auto toCols = index.data(CommitModel::ParentGraphColumns).value<QList<int>>();
    auto passthroughCols = index.data(CommitModel::PassthroughGraphColumns).value<QList<int>>();

    // Find the extremities of the commit graph
    int commitCol = fromCols.first();
    int extremCol = fromCols.first();
    for (auto col : fromCols) commitCol = qMin(commitCol, col);
    for (auto col : fromCols) extremCol = qMax(extremCol, col);
    for (auto col : toCols) extremCol = qMax(extremCol, col);
    for (auto col : passthroughCols) extremCol = qMax(extremCol, col);

    QRectF dataRect = option.rect;
    dataRect.setLeft(dataRect.left() + 20 * extremCol + 26);

    QString message = index.data(CommitModel::CommitMessage).toString();
    QRectF messageBox;
    messageBox.setHeight(option.fontMetrics.height());
    messageBox.setWidth(dataRect.width() - 18);
    messageBox.moveTopLeft(dataRect.topLeft() + QPointF(9, 9));

    QStringList subtext;
    subtext.append(index.data(CommitModel::AuthorName).toString());
    subtext.append(QLocale().toString(index.data(CommitModel::CommitDate).toDateTime()));
    subtext.append(index.data(CommitModel::ShortCommitHash).toString());
    QString author = subtext.join(libContemporaryCommon::humanReadablePartJoinString());

    QRectF authorNameBox;
    authorNameBox.setHeight(lowerFontMetrics.height());
    authorNameBox.setWidth(lowerFontMetrics.horizontalAdvance(author) + 2);
    authorNameBox.moveLeft(messageBox.left());
    authorNameBox.moveTop(messageBox.bottom() + 3);

    QRectF bounding(messageBox.topLeft(), authorNameBox.bottomRight());

    auto mainCommitPoint = this->commitPoint(commitCol, bounding, option.rect.left(), option.widget);
    calculator.addRect(mainCommitPoint, [painter, option](QRectF drawBounds) {
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setBrush(option.palette.color(QPalette::WindowText));
        painter->drawEllipse(drawBounds);
    });

    for (auto from : fromCols) {
        auto point = this->commitPoint(from, bounding, option.rect.left(), option.widget);
        calculator.addRect(QRect::span(mainCommitPoint.center().toPoint(), QPoint(point.center().toPoint().x(), option.rect.top())), [painter, option](QRectF drawBounds) {
            painter->setRenderHint(QPainter::Antialiasing, false);
            painter->setPen(option.palette.color(QPalette::WindowText));
            painter->drawLine(drawBounds.topRight().toPoint(), drawBounds.bottomLeft().toPoint());
        });
    }

    for (auto to : toCols) {
        auto point = this->commitPoint(to, bounding, option.rect.left(), option.widget);
        calculator.addRect(QRect::span(mainCommitPoint.center().toPoint(), QPoint(point.center().toPoint().x(), option.rect.top())).translated(0, point.center().y() - option.rect.top()), [painter, option](QRectF drawBounds) {
            painter->setRenderHint(QPainter::Antialiasing, false);
            painter->setPen(option.palette.color(QPalette::WindowText));
            painter->drawLine(drawBounds.topLeft().toPoint(), drawBounds.bottomRight().toPoint());
        });
    }

    for (auto passthrough : passthroughCols) {
        auto point = this->commitPoint(passthrough, bounding, option.rect.left(), option.widget);
        calculator.addRect(QRect::span(QPoint(point.center().toPoint().x(), option.rect.top()), QPoint(point.center().toPoint().x(), option.rect.top() + (point.center().y() - option.rect.top()) * 2)), [painter, option](QRectF drawBounds) {
            painter->setRenderHint(QPainter::Antialiasing, false);
            painter->setPen(option.palette.color(QPalette::WindowText));
            painter->drawLine(drawBounds.topLeft().toPoint(), drawBounds.bottomRight().toPoint());
        });
    }

    calculator.addRect("name", messageBox, [&, message, painter, option](QRectF drawBounds) {
        painter->drawText(drawBounds, option.fontMetrics.elidedText(message, Qt::ElideRight, drawBounds.width()));
    });

    calculator.addRect("author", authorNameBox, [&, author, painter, lowerFont](QRectF drawBounds) {
        painter->setFont(lowerFont);
        painter->drawText(drawBounds, author);
    });

    return calculator;
}

QRectF CommitDelegate::commitPoint(int col, QRectF bounding, int left, const QWidget* parent) const {
    QRectF commitPoint;
    commitPoint.setSize(QSizeF(16, 16));
    commitPoint.moveCenter(QPoint(left + 20 * col + 15, bounding.center().y()));
    return commitPoint;
}
