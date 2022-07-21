#include "stashesmodel.h"

#include "repository.h"
#include "stash.h"

struct StashesModelPrivate {
        RepositoryPtr repo;
        QList<StashPtr> stashes;
};

StashesModel::StashesModel(QObject* parent) :
    QAbstractListModel(parent) {
    d = new StashesModelPrivate();
}

StashesModel::~StashesModel() {
    delete d;
}

int StashesModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return d->stashes.length();
}

QVariant StashesModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) return QVariant();

    auto stash = d->stashes.at(index.row());
    switch (role) {
        case Qt::DisplayRole:
            return stash->message();
        case Stash:
            return QVariant::fromValue(stash);
    }

    return QVariant();
}

void StashesModel::setRepository(RepositoryPtr repo) {
    d->repo = repo;
    connect(repo.data(), &Repository::repositoryUpdated, this, &StashesModel::reloadData);
    reloadData();
}

void StashesModel::reloadData() {
    if (!d->repo) return;
    if (!d->repo->git_repository()) return;

    d->stashes = d->repo->stashes();
    emit dataChanged(index(0, 0), index(rowCount()));
}
