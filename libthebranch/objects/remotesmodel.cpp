/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2022 Victor Tran
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * *************************************/
#include "remotesmodel.h"

#include "remote.h"
#include "repository.h"

struct RemotesModelPrivate {
        RepositoryPtr repo;
        QList<RemotePtr> remotes;
};

RemotesModel::RemotesModel(QObject* parent) :
    QAbstractListModel(parent) {
    d = new RemotesModelPrivate();
}

RemotesModel::~RemotesModel() {
    delete d;
}

int RemotesModel::rowCount(const QModelIndex& parent) const {
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return d->remotes.count();
}

QVariant RemotesModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();

    auto remote = d->remotes.at(index.row());
    switch (role) {
        case Qt::DisplayRole:
            return remote->name();
    }

    return QVariant();
}

void RemotesModel::setRepository(RepositoryPtr repo) {
    d->repo = repo;
    connect(repo.data(), &Repository::repositoryUpdated, this, &RemotesModel::reloadData);
    reloadData();
}

void RemotesModel::reloadData() {
    if (!d->repo) return;
    if (!d->repo->git_repository()) return;

    d->remotes = d->repo->remotes();
    emit dataChanged(index(0, 0), index(rowCount()));
}
