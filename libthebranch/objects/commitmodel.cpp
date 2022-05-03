#include "commitmodel.h"

#include "commit.h"
#include "libgit/lgcommit.h"
#include "libgit/lgreference.h"
#include "libgit/lgrepository.h"
#include "libgit/lgrevwalk.h"
#include "repository.h"
#include <QPainter>
#include <git2.h>

struct CommitModelPrivate {
        RepositoryPtr repo = nullptr;
        QList<CommitPtr> commits;

        QString startPoint = "HEAD";
};

CommitModel::CommitModel(QObject* parent) :
    QAbstractListModel(parent) {
    d = new CommitModelPrivate();
}

CommitModel::~CommitModel() {
    delete d;
}

void CommitModel::setStartPoint(QString startPoint) {
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

    LGOidPtr headOid = LGReference::nameToId(d->repo->git_repository(), d->startPoint);

    LGRevwalkPtr revwalk = LGRevwalk::revwalk_new(d->repo->git_repository());
    revwalk->sorting(GIT_SORT_TOPOLOGICAL);
    revwalk->push(headOid);

    QList<CommitPtr> commits;
    QList<LGOidPtr> oids = revwalk->walk();
    for (LGOidPtr oid : oids) {
        LGCommitPtr commit = LGCommit::lookup(d->repo->git_repository(), oid);
        commits.append(CommitPtr(Commit::commitForLgCommit(commit)));
    }

    d->commits = commits;
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
        case AuthorName:
            return commit->authorName();
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

    QString message = index.data(CommitModel::CommitMessage).toString();
    QRectF messageBox;
    messageBox.setHeight(option.fontMetrics.height());
    messageBox.setWidth(option.rect.width() - SC_DPI_W(18, option.widget));
    messageBox.moveTopLeft(option.rect.topLeft() + SC_DPI_WT(QPointF(9, 9), QPointF, option.widget));
    calculator.addRect("name", messageBox, [&, message, painter, option](QRectF drawBounds) {
        painter->drawText(drawBounds, option.fontMetrics.elidedText(message, Qt::ElideRight, drawBounds.width()));
    });

    QFont lowerFont = option.font;
    lowerFont.setPointSize(lowerFont.pointSizeF() * 0.8);
    QFontMetrics lowerFontMetrics(lowerFont);

    QString author = index.data(CommitModel::AuthorName).toString();
    QRectF authorNameBox;
    authorNameBox.setHeight(lowerFontMetrics.height());
    authorNameBox.setWidth(lowerFontMetrics.horizontalAdvance(author) + 1);
    authorNameBox.moveLeft(messageBox.left());
    authorNameBox.moveTop(messageBox.bottom() + SC_DPI_W(3, option.widget));
    calculator.addRect("author", authorNameBox, [&, author, painter, lowerFont](QRectF drawBounds) {
        painter->setFont(lowerFont);
        painter->drawText(drawBounds, author);
    });

    return calculator;
}
