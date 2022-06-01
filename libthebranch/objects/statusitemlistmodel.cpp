#include "statusitemlistmodel.h"

struct StatusItemListModelPrivate {
        QList<Repository::StatusItem> statusItems;
        QSet<QPersistentModelIndex> checkedItems;

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

    return d->statusItems.count();
}

QVariant StatusItemListModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();

    Repository::StatusItem statusItem = d->statusItems.at(index.row());
    switch (role) {
        case Qt::CheckStateRole:
            return d->checkedItems.contains(index) ? Qt::Checked : Qt::Unchecked;
        case Qt::DisplayRole:
        case PathRole:
            return statusItem.path;
        case StatusRole:
            return statusItem.flags;
    }
    return QVariant();
}

QModelIndexList StatusItemListModel::checkedItems() {
    QModelIndexList indices;
    for (QPersistentModelIndex index : d->checkedItems) {
        indices.append(index);
    }
    return indices;
}

void StatusItemListModel::setStatusItems(QList<Repository::StatusItem> items) {
    d->statusItems = items;
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

bool StatusItemListFilterView::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const {
    QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
    int status = index.data(StatusItemListModel::StatusRole).toInt();

    for (int statusFlag = Repository::StatusItem::FirstStatusFlag; statusFlag <= Repository::StatusItem::LastStatusFlag; statusFlag <<= 1) {
        if (d->flagFilters & statusFlag && status & statusFlag) return true;
    }

    return false;
}

StatusItemListDelegate::StatusItemListDelegate(QObject* parent) :
    QStyledItemDelegate(parent) {
}

tPaintCalculator StatusItemListDelegate::paintCalculator(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) {
    return tPaintCalculator();
}

void StatusItemListDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QStyledItemDelegate::paint(painter, option, index);
}

QSize StatusItemListDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
    return QStyledItemDelegate::sizeHint(option, index);
}

Qt::ItemFlags StatusItemListModel::flags(const QModelIndex& index) const {
    Qt::ItemFlags flags = QAbstractListModel::flags(index);
    if (d->userCheckable) flags |= Qt::ItemIsUserCheckable;
    return flags;
}

bool StatusItemListModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (!index.isValid()) return false;
    if (role != Qt::CheckStateRole) return false;

    if (value == Qt::Checked) {
        d->checkedItems.insert(index);
    } else {
        d->checkedItems.remove(index);
    }

    emit dataChanged(index, index);
    emit checkedItemsChanged();
    return true;
}
