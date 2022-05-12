#include "indexmodel.h"

struct IndexModelPrivate {
        IndexPtr index;
};

IndexModel::IndexModel(QObject* parent) :
    QAbstractListModel(parent) {
    d = new IndexModelPrivate;
}

IndexModel::~IndexModel() {
    delete d;
}

void IndexModel::setIndex(IndexPtr index) {
    d->index = index;
}

void IndexModel::reloadData() {
    if (!d->index) return;

    emit dataChanged(index(0, 0), index(rowCount()));
}

int IndexModel::rowCount(const QModelIndex& parent) const {
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return 0;
}

QVariant IndexModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();

    // FIXME: Implement me!
    return QVariant();
}
