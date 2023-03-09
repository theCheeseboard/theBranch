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
#ifndef REMOTESMODEL_H
#define REMOTESMODEL_H

#include "../libthebranch_global.h"
#include "forward_declares.h"
#include <QAbstractListModel>

struct RemotesModelPrivate;
class LIBTHEBRANCH_EXPORT RemotesModel : public QAbstractListModel {
        Q_OBJECT

    public:
        explicit RemotesModel(QObject* parent = nullptr);
        ~RemotesModel();

        enum RemotesModelRole {
            Remote = Qt::UserRole
        };

        // Basic functionality:
        int rowCount(const QModelIndex& parent = QModelIndex()) const override;

        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

        void setRepository(RepositoryPtr repo);
        void reloadData();

    private:
        RemotesModelPrivate* d;
};

#endif // REMOTESMODEL_H
