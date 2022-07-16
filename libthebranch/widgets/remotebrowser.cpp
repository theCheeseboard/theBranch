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
#include "remotebrowser.h"

#include "objects/remotesmodel.h"
#include <QContextMenuEvent>
#include <QMenu>

struct RemoteBrowserPrivate {
        RepositoryPtr repo;
        RemotesModel* model;
};

RemoteBrowser::RemoteBrowser(QWidget* parent) :
    QListView{parent} {
    d = new RemoteBrowserPrivate();

    d->model = new RemotesModel();
    this->setModel(d->model);
    //    this->setItemDelegate(new CommitDelegate(this));
    this->setFrameShape(QFrame::NoFrame);
}

RemoteBrowser::~RemoteBrowser() {
    delete d;
}

void RemoteBrowser::setRepository(RepositoryPtr repo) {
    d->repo = repo;
    d->model->setRepository(repo);
}

void RemoteBrowser::contextMenuEvent(QContextMenuEvent* event) {
    if (this->selectedIndexes().isEmpty()) return;

    QModelIndex index = this->selectedIndexes().first();

    QMenu* menu = new QMenu();

    connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
    menu->popup(event->globalPos());
}
