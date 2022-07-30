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

#include "commitbrowserwidget.h"
#include "objects/branch.h"
#include "objects/branchuihelper.h"
#include "objects/remote.h"
#include "objects/repository.h"
#include "objects/repositorymodel.h"
#include "objects/stash.h"
#include "popovers/snapinpopover.h"
#include "popovers/snapins/newremotesnapin.h"
#include "popovers/snapins/stashsavesnapin.h"
#include <QContextMenuEvent>
#include <QMenu>
#include <QStandardItemModel>

struct RepositoryBrowserListPrivate {
        //        RepositoryModel* model;
        QStandardItemModel* model;
        RepositoryPtr repo;

        QStandardItem *branchParent, *remoteParent, *stashParent;

        QList<BranchPtr> branches;
        QList<BranchPtr> remoteBranches;
        QList<RemotePtr> remotes;
        QList<StashPtr> stashes;

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
    d->stashParent = new QStandardItem(QIcon::fromTheme("vcs-stash"), tr("Stashes"));

    auto rootItem = d->model->invisibleRootItem();
    rootItem->appendRows({d->branchParent, d->stashParent, d->remoteParent});

    connect(this, &QTreeView::clicked, this, [this](QModelIndex index) {
        auto itemData = index.data(Qt::UserRole + 1).value<QSharedPointer<QObject>>();
        if (auto branch = itemData.objectCast<Branch>()) {
            auto commitBrowser = new CommitBrowserWidget();
            commitBrowser->setRepository(d->repo);
            commitBrowser->setStartBranch(branch);
            emit showWidget(commitBrowser);
        }
    });

    this->setHeaderHidden(true);
}

RepositoryBrowserList::~RepositoryBrowserList() {
    delete d;
}

void RepositoryBrowserList::setRepository(RepositoryPtr repo) {
    d->repo = repo;
    connect(repo.data(), &Repository::repositoryUpdated, this, &RepositoryBrowserList::updateData);
    this->updateData();
}

void RepositoryBrowserList::updateData() {
    d->branches = d->repo->branches(THEBRANCH::LocalBranches);
    d->remoteBranches = d->repo->branches(THEBRANCH::RemoteBranches);
    d->remotes = d->repo->remotes();
    d->stashes = d->repo->stashes();

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

        for (auto remoteBranch : d->remoteBranches) {
            if (remoteBranch->remoteName() == remote->name()) {
                auto branchItem = new QStandardItem(remoteBranch->name());
                branchItem->setData(QVariant::fromValue(remoteBranch.staticCast<QObject>()));
                item->appendRow(branchItem);
            }
        }
    }

    d->stashParent->removeRows(0, d->stashParent->rowCount());
    for (auto stash : d->stashes) {
        auto item = new QStandardItem(stash->message());
        item->setData(QVariant::fromValue(stash.staticCast<QObject>()));
        d->stashParent->appendRow(item);
    }
}

void RepositoryBrowserList::setBeforeActionPerformedHandler(std::function<QCoro::Task<>()> handler) {
    d->handler = handler;
}

QCoro::Task<> RepositoryBrowserList::showContextMenu(QPoint pos) {
    auto index = this->indexAt(pos);
    if (!index.isValid()) co_return;

    if (d->handler) co_await d->handler();

    QMenu* menu = new QMenu();

    auto item = d->model->itemFromIndex(index);
    auto itemData = index.data(Qt::UserRole + 1).value<QSharedPointer<QObject>>();

    if (item == d->remoteParent) {
        menu->addSection(tr("For repository"));
        menu->addAction(QIcon::fromTheme("list-add"), tr("Add Remote"), this, [this] {
            SnapInPopover::showSnapInPopover(this->window(), new NewRemoteSnapIn(d->repo));
        });
    } else if (item == d->stashParent) {
        menu->addSection(tr("For repository"));
        menu->addAction(QIcon::fromTheme("vcs-stash"), tr("Stash"), this, [this] {
            SnapInPopover::showSnapInPopover(this->window(), new StashSaveSnapIn(d->repo));
        });
    } else if (auto branch = itemData.objectCast<Branch>()) {
        BranchUiHelper::appendBranchMenu(menu, branch, d->repo, this);
    } else if (auto remote = itemData.objectCast<Remote>()) {
        BranchUiHelper::appendRemoteMenu(menu, remote, d->repo, this);
    } else if (auto stash = itemData.objectCast<Stash>()) {
        BranchUiHelper::appendStashMenu(menu, stash, d->repo, this);
    }

    connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
    menu->popup(this->mapToGlobal(pos));
}

void RepositoryBrowserList::contextMenuEvent(QContextMenuEvent* event) {
    this->showContextMenu(event->pos());
}
