#include "branchmodel.h"

#include "branch.h"
#include "remote.h"
#include "repository.h"
#include <git2.h>

struct BranchModelPrivate {
        RepositoryPtr repo;
        QList<BranchPtr> branches;
        QList<RemotePtr> remotes;
        THEBRANCH::ListBranchFlags flags = THEBRANCH::AllBranches;

        QString fakePushBranchName;
};

BranchModel::BranchModel(QObject* parent) :
    QAbstractListModel(parent) {
    d = new BranchModelPrivate();
}

BranchModel::~BranchModel() {
    delete d;
}

int BranchModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return d->branches.length() + this->nonexistentRemoteNames().length();
}

QVariant BranchModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) return QVariant();

    auto nonexistentRemoteNames = this->nonexistentRemoteNames();

    if (index.row() >= nonexistentRemoteNames.length()) {
        BranchPtr branch = d->branches.at(index.row() - nonexistentRemoteNames.length());
        switch (role) {
            case Qt::DisplayRole:
                return branch->name();
            case Branch:
                return QVariant::fromValue(branch);
            case FakeRemoteName:
                return "";
        }
    } else {
        auto fakeName = nonexistentRemoteNames.at(index.row());
        switch (role) {
            case Qt::DisplayRole:
                return tr("New branch %1 on remote %2").arg(QLocale().quoteString(d->fakePushBranchName), QLocale().quoteString(fakeName));
            case Branch:
                return QVariant::fromValue(BranchPtr(nullptr));
            case FakeRemoteName:
                return fakeName;
        }
    }

    return QVariant();
}

QModelIndex BranchModel::index(BranchPtr branch) {
    for (auto i = 0; i < d->branches.length(); i++) {
        if (d->branches.at(i)->equal(branch)) return index(i + this->nonexistentRemoteNames().length());
    }
    return QModelIndex();
}

void BranchModel::setFakePushBranchName(QString name) {
    d->fakePushBranchName = name;
    emit dataChanged(index(0, 0), index(rowCount()));
}

QStringList BranchModel::nonexistentRemoteNames() const {
    if (d->fakePushBranchName.isEmpty()) return {};

    QStringList nonexistentRemoteNames;
    for (auto remote : d->remotes) {
        bool found = false;
        for (auto branch : d->branches) {
            if (branch->name() == QStringLiteral("%1/%2").arg(remote->name(), d->fakePushBranchName)) {
                found = true;
            }
        }

        if (!found) nonexistentRemoteNames.append(remote->name());
    }
    return nonexistentRemoteNames;
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
    d->remotes = d->repo->remotes();
    emit dataChanged(index(0, 0), index(rowCount()));
}

void BranchModel::setBranchFlags(THEBRANCH::ListBranchFlags flags) {
    d->flags = flags;
    reloadData();
}

QModelIndex BranchModel::index(int row, int column, const QModelIndex& parent) const {
    return QAbstractListModel::index(row, column, parent);
}
