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
#include "repositorybrowserlist.h"

#include "objects/branch.h"
#include "objects/branchuihelper.h"
#include "objects/remote.h"
#include "objects/repository.h"
#include "objects/repositorymodel.h"
#include <QContextMenuEvent>
#include <QMenu>
#include <QStandardItemModel>

struct RepositoryBrowserListPrivate {
        //        RepositoryModel* model;
        QStandardItemModel* model;
        RepositoryPtr repo;

        QStandardItem *branchParent, *remoteParent;

        QList<BranchPtr> branches;
        QList<RemotePtr> remotes;

        std::function<QCoro::Task<>()> handler = nullptr;
};

RepositoryBrowserList::RepositoryBrowserList(QWidget* parent) :
    QTreeView{parent} {
    d = new RepositoryBrowserListPrivate();

    d->model = new QStandardItemModel();
    //    d->model = new RepositoryModel();
    this->setModel(d->model);
    //    this->setItemDelegate(new CommitDelegate(this));
    this->setFrameShape(QFrame::NoFrame);

    d->branchParent = new QStandardItem(QIcon::fromTheme("vcs-branch"), tr("Branches"));
    d->remoteParent = new QStandardItem(QIcon::fromTheme("cloud-download"), tr("Remotes"));

    auto rootItem = d->model->invisibleRootItem();
    rootItem->appendRows({d->branchParent, d->remoteParent});
}

RepositoryBrowserList::~RepositoryBrowserList() {
    delete d;
}

void RepositoryBrowserList::setRepository(RepositoryPtr repo) {
    d->repo = repo;
    d->branches = d->repo->branches(THEBRANCH::LocalBranches);
    d->remotes = d->repo->remotes();
    this->updateData();
    //    d->model->setRepository(repo);
}

void RepositoryBrowserList::updateData() {
    d->branchParent->removeRows(0, d->branchParent->rowCount());
    for (auto branch : d->branches) {
        auto item = new QStandardItem(branch->name());
        item->setData(QVariant::fromValue(branch.staticCast<QObject>()));
        d->branchParent->appendRow(item);
    }

    d->remoteParent->removeRows(0, d->remoteParent->rowCount());
    for (auto remote : d->remotes) {
        auto item = new QStandardItem(remote->name());
        item->setData(QVariant::fromValue(remote.staticCast<QObject>()));
        d->remoteParent->appendRow(item);
    }
}

void RepositoryBrowserList::setBeforeActionPerformedHandler(std::function<QCoro::Task<>()> handler) {
}

QCoro::Task<> RepositoryBrowserList::showContextMenu(QPoint pos) {
    auto index = this->indexAt(pos);
    if (!index.isValid()) co_return;

    if (d->handler) co_await d->handler();

    QMenu* menu = new QMenu();

    auto itemData = index.data(Qt::UserRole + 1).value<QSharedPointer<QObject>>();
    if (auto branch = itemData.objectCast<Branch>()) {
        BranchUiHelper::appendBranchMenu(menu, branch, d->repo, this);
    } else if (auto remote = itemData.objectCast<Remote>()) {
    }

    connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
    menu->popup(this->mapToGlobal(pos));
}

void RepositoryBrowserList::contextMenuEvent(QContextMenuEvent* event) {
    this->showContextMenu(event->pos());
}
