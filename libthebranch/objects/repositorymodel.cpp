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
#include "repositorymodel.h"

#include "objects/remote.h"
#include "objects/repository.h"

struct RepositoryModelPrivate {
        RepositoryPtr repo;

        QList<std::tuple<QIcon, QString, std::function<int()>, std::function<QVariant(const QModelIndex&, int role)>>> parentItems;

        QList<BranchPtr> branches;
        QList<RemotePtr> remotes;
};

RepositoryModel::RepositoryModel(QObject* parent) :
    QAbstractItemModel(parent) {
    d = new RepositoryModelPrivate();

    d->parentItems.append({
        {QIcon::fromTheme("vcs-branch"), tr("Branches"), [this] {
             return d->branches.length();
         },
         [this](const QModelIndex& index, int role) -> QVariant {
                auto branch = d->branches.at(index.row());
                switch (role) {
                    case Qt::DisplayRole:
                        return branch->name();
                }

                return QVariant();
            }},
    });

    d->parentItems.append({
        {QIcon::fromTheme("cloud-download"), tr("Remotes"), [this] {
             return d->remotes.length();
         },
         [this](const QModelIndex& index, int role) -> QVariant {
                auto remote = d->remotes.at(index.row());
                switch (role) {
                    case Qt::DisplayRole:
                        return remote->name();
                }

                return QVariant();
            }},
    });
}

RepositoryModel::~RepositoryModel() {
    delete d;
}

int RepositoryModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        auto [icon, text, count, get] = d->parentItems.at(parent.row());
        return count();
    }

    return d->parentItems.length();
}

QVariant RepositoryModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) return QVariant();

    if (index.parent().isValid()) {
        auto [icon, text, count, get] = d->parentItems.at(index.parent().row());
        return get(index, role);
    }

    auto [icon, text, count, get] = d->parentItems.at(index.row());
    switch (role) {
        case Qt::DisplayRole:
            return text;
        case Qt::DecorationRole:
            return icon;
    }

    return QVariant();
}

QModelIndex RepositoryModel::index(int row) const {
    return index(row, 0, QModelIndex());
}

QModelIndex RepositoryModel::index(int row, const QModelIndex& parent) const {
    return index(row, 0, parent);
}

void RepositoryModel::setRepository(RepositoryPtr repo) {
    d->repo = repo;
    connect(repo.data(), &Repository::repositoryUpdated, this, &RepositoryModel::reloadData);
    reloadData();
}

void RepositoryModel::reloadData() {
    if (!d->repo) return;

    d->branches = d->repo->branches(THEBRANCH::LocalBranches);
    d->remotes = d->repo->remotes();
    emit dataChanged(index(0, QModelIndex()), index(rowCount(QModelIndex()) - 1, QModelIndex()));
    for (auto i = 0; i < d->parentItems.length(); i++) {
        auto parentIndex = index(i);
        emit dataChanged(index(0, parentIndex), index(rowCount(parentIndex) - 1, parentIndex));
    }
}

QModelIndex RepositoryModel::index(int row, int column, const QModelIndex& parent) const {
    return createIndex(row, column, parent.isValid() ? parent.row() : -1);
}

QModelIndex RepositoryModel::parent(const QModelIndex& child) const {
    auto id = child.internalId();
    if (id == -1) return QModelIndex();

    return this->index(id, 0, QModelIndex());
}

int RepositoryModel::columnCount(const QModelIndex& parent) const {
    return 1;
}
