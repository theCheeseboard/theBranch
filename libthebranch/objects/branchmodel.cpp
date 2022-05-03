#include "branchmodel.h"

#include "branch.h"
#include "repository.h"
#include <git2.h>

struct BranchModelPrivate {
        RepositoryPtr repo;
        QList<BranchPtr> branches;
        THEBRANCH::ListBranchFlags flags = THEBRANCH::AllBranches;
};

BranchModel::BranchModel(QObject* parent) :
    QAbstractListModel(parent) {
    d = new BranchModelPrivate();
}

BranchModel::~BranchModel() {
    delete d;
}

int BranchModel::rowCount(const QModelIndex& parent) const {
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    // FIXME: Implement me!
    return d->branches.length();
}

QVariant BranchModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();

    BranchPtr branch = d->branches.at(index.row());
    switch (role) {
        case Qt::DisplayRole:
            return branch->name();
    }

    return QVariant();
}

void BranchModel::setRepository(RepositoryPtr repo) {
    d->repo = repo;
    connect(repo.data(), &Repository::repositoryUpdated, this, &BranchModel::reloadData);
    reloadData();
}

void BranchModel::reloadData() {
    if (!d->repo) return;
    if (!d->repo->git_repository()) return;

    d->branches = d->repo->branches(d->flags);
    emit dataChanged(index(0), index(rowCount()));
}

void BranchModel::setBranchFlags(THEBRANCH::ListBranchFlags flags) {
    d->flags = flags;
    reloadData();
}
