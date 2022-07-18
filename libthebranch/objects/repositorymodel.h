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
#ifndef REPOSITORYMODEL_H
#define REPOSITORYMODEL_H

#include "forward_declares.h"
#include <QAbstractItemModel>

struct RepositoryModelPrivate;
class RepositoryModel : public QAbstractItemModel {
        Q_OBJECT

    public:
        explicit RepositoryModel(QObject* parent = nullptr);
        ~RepositoryModel();

        // Basic functionality:
        int rowCount(const QModelIndex& parent = QModelIndex()) const override;

        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

        QModelIndex index(int row) const;
        QModelIndex index(int row, const QModelIndex& parent) const;

        void setRepository(RepositoryPtr repo);
        void reloadData();

    private:
        RepositoryModelPrivate* d;

        // QAbstractItemModel interface
    public:
        QModelIndex index(int row, int column, const QModelIndex& parent) const;
        QModelIndex parent(const QModelIndex& child) const;
        int columnCount(const QModelIndex& parent) const;
};

#endif // REPOSITORYMODEL_H
