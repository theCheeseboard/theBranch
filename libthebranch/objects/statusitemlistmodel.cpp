#include "statusitemlistmodel.h"

#include <QPainter>

struct StatusItemListModelPrivate {
        QList<Repository::StatusItem> statusItems;
        QList<Diff::DiffFile> diffResults;
        QSet<QString> checkedItems;

        bool userCheckable = true;
};

StatusItemListModel::StatusItemListModel(QObject* parent) :
    QAbstractListModel(parent) {
    d = new StatusItemListModelPrivate();
}

StatusItemListModel::~StatusItemListModel() {
    delete d;
}

int StatusItemListModel::rowCount(const QModelIndex& parent) const {
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    if (d->diffResults.isEmpty()) return d->statusItems.count();
    return d->diffResults.count();
}

QVariant StatusItemListModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (!d->diffResults.isEmpty()) {
        auto diffResult = d->diffResults.at(index.row());
        switch (role) {
            case Qt::CheckStateRole:
                return Qt::Unchecked;
            case Qt::DisplayRole:
            case PathRole:
                return diffResult.oldFilePath;
            case RenamedPathRole:
                return diffResult.newFilePath;
            case StatusRole:
                return diffResult.statusFlag;
            case BlobOldSide:
                return diffResult.oldBlob;
            case BlobNewSide:
                return diffResult.newBlob;
        }
    } else if (!d->statusItems.isEmpty()) {
        auto statusItem = d->statusItems.at(index.row());
        switch (role) {
            case Qt::CheckStateRole:
                return d->checkedItems.contains(statusItem.path) ? Qt::Checked : Qt::Unchecked;
            case Qt::DisplayRole:
            case PathRole:
                return statusItem.path;
            case RenamedPathRole:
                return statusItem.path;
            case StatusRole:
                return statusItem.flags;
        }
    }
    return QVariant();
}

QModelIndexList StatusItemListModel::checkedItems() {
    QModelIndexList indices;
    for (const auto& path : qAsConst(d->checkedItems)) {
        for (auto i = 0; i < d->statusItems.length(); i++) {
            if (d->statusItems.at(i).path == path) {
                indices.append(index(i));
                break;
            }
        }
    }
    return indices;
}

void StatusItemListModel::setStatusItems(QList<Repository::StatusItem> items) {
    d->statusItems = items;

    // Remove any checked items without a corresponding status item
    d->checkedItems.removeIf([this](QString path) {
        for (auto i = 0; i < d->statusItems.length(); i++) {
            if (d->statusItems.at(i).path == path) return false;
        }
        return true;
    });
    emit dataChanged(index(0), index(rowCount()));
    d->diffResults.clear();
}

void StatusItemListModel::setDiffResult(QList<Diff::DiffFile> results) {
    d->statusItems.clear();
    d->diffResults = results;

    d->checkedItems.clear();
    emit dataChanged(index(0), index(rowCount()));
}

void StatusItemListModel::setUserCheckable(bool userCheckable) {
    d->userCheckable = userCheckable;
    emit dataChanged(index(0), index(rowCount()));
}

struct StatusItemListFilterViewPrivate {
        int flagFilters;
};

StatusItemListFilterView::StatusItemListFilterView(QObject* parent) :
    QSortFilterProxyModel(parent) {
    d = new StatusItemListFilterViewPrivate();
}

StatusItemListFilterView::~StatusItemListFilterView() {
    delete d;
}

void StatusItemListFilterView::setFlagFilters(int flagFilters) {
    d->flagFilters = flagFilters;
    invalidateFilter();
}

bool StatusItemListFilterView::acceptRowWithFlags(int flagFilters, int source_row, const QModelIndex& source_parent) const {
    QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
    int status = index.data(StatusItemListModel::StatusRole).toInt();

    for (int statusFlag = Repository::StatusItem::FirstStatusFlag; statusFlag <= Repository::StatusItem::LastStatusFlag; statusFlag <<= 1) {
        if (flagFilters & statusFlag && status & statusFlag) return true;
    }

    return false;
}

bool StatusItemListFilterView::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const {
    return acceptRowWithFlags(d->flagFilters, source_row, source_parent);
}

StatusItemListDelegate::StatusItemListDelegate(QObject* parent) :
    QStyledItemDelegate(parent) {
}

tPaintCalculator StatusItemListDelegate::paintCalculator(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) {
    return tPaintCalculator();
}

void StatusItemListDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QStyledItemDelegate::paint(painter, option, index);

    QStringList statusText;
    auto status = index.data(StatusItemListModel::StatusRole).toInt();

    if (status & Repository::StatusItem::New) statusText.append("N");
    if (status & Repository::StatusItem::Deleted) statusText.append("D");
    if (status & Repository::StatusItem::Modified) statusText.append("M");
    if (status & Repository::StatusItem::Renamed) statusText.append("R");
    if (status & Repository::StatusItem::Conflicting) statusText.append("C");

    QRect statusTextRect;
    statusTextRect.setHeight(painter->fontMetrics().height());
    statusTextRect.setWidth(painter->fontMetrics().horizontalAdvance(statusText.join(" ")));
    statusTextRect.moveRight(option.rect.right() - SC_DPI_W(9, option.widget));
    statusTextRect.moveTop(option.rect.top() + option.rect.height() / 2 - statusTextRect.height() / 2);
    painter->drawText(statusTextRect, Qt::AlignVCenter | Qt::AlignRight, statusText.join(" "));
}

QSize StatusItemListDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
    return QStyledItemDelegate::sizeHint(option, index);
}

Qt::ItemFlags StatusItemListModel::flags(const QModelIndex& index) const {
    Qt::ItemFlags flags = QAbstractListModel::flags(index);
    if (d->userCheckable) {
        flags |= Qt::ItemIsUserCheckable;
    } else {
        flags &= ~Qt::ItemIsUserCheckable;
    }
    return flags;
}

bool StatusItemListModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (!index.isValid()) return false;
    if (role != Qt::CheckStateRole) return false;

    if (value == Qt::Checked) {
        d->checkedItems.insert(index.data(PathRole).toString());
    } else {
        d->checkedItems.remove(index.data(PathRole).toString());
    }

    emit dataChanged(index, index);
    emit checkedItemsChanged();
    return true;
}
