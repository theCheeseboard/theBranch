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
#ifndef REPOSITORYBROWSERLIST_H
#define REPOSITORYBROWSERLIST_H

#include "objects/forward_declares.h"
#include <QCoroTask>
#include <QTreeView>

struct RepositoryBrowserListPrivate;
class RepositoryBrowserList : public QTreeView {
        Q_OBJECT
    public:
        explicit RepositoryBrowserList(QWidget* parent = nullptr);
        ~RepositoryBrowserList();

        void setRepository(RepositoryPtr repo);
        void updateData();

        void setBeforeActionPerformedHandler(std::function<QCoro::Task<>()> handler);

    private:
        RepositoryBrowserListPrivate* d;

        QCoro::Task<> showContextMenu(QPoint pos);

        // QWidget interface
    protected:
        void contextMenuEvent(QContextMenuEvent* event);
};

#endif // REPOSITORYBROWSERLIST_H
