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

#include "accounts/accountsmanager.h"
#include "accounts/github/githubaccount.h"
#include "accounts/github/issues/githubissuelistcontroller.h"
#include "accounts/github/pr/githubpullrequestlistcontroller.h"
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

        QStandardItem *branchParent, *remoteParent, *stashParent, *issuesParent, *prsParent;

        QList<BranchPtr> branches;
        QList<BranchPtr> remoteBranches;
        QList<RemotePtr> remotes;
        QList<StashPtr> stashes;

        QList<GitHubIssueListController*> ghIssueControllers;
        QList<GitHubPullRequestListController*> ghPrControllers;

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
    this->setEditTriggers(NoEditTriggers);

    d->branchParent = new QStandardItem(QIcon::fromTheme("vcs-branch"), tr("Branches"));
    d->remoteParent = new QStandardItem(QIcon::fromTheme("cloud-download"), tr("Remotes"));
    d->stashParent = new QStandardItem(QIcon::fromTheme("vcs-stash"), tr("Stashes"));
    d->issuesParent = new QStandardItem(QIcon::fromTheme("tools-report-bug"), tr("Issues"));
    d->prsParent = new QStandardItem(QIcon::fromTheme("vcs-pull-request"), tr("Pull Requests"));

    addContextMenuFunction(d->remoteParent, [this](QMenu* menu) {
        menu->addSection(tr("For repository"));
        menu->addAction(QIcon::fromTheme("list-add"), tr("Add Remote"), this, [this] {
            SnapInPopover::showSnapInPopover(this->window(), new NewRemoteSnapIn(d->repo));
        });
    });
    addContextMenuFunction(d->stashParent, [this](QMenu* menu) {
        menu->addSection(tr("For repository"));
        menu->addAction(QIcon::fromTheme("vcs-stash"), tr("Stash"), this, [this] {
            SnapInPopover::showSnapInPopover(this->window(), new StashSaveSnapIn(d->repo));
        });
    });

    auto rootItem = d->model->invisibleRootItem();
    rootItem->appendRows({d->branchParent, d->stashParent, d->remoteParent, d->issuesParent, d->prsParent});

    connect(this, &QTreeView::clicked, this, [this](QModelIndex index) {
        auto widgetFn = index.data(static_cast<int>(Roles::WidgetFunction)).value<WidgetFunction>();
        if (widgetFn) {
            auto widget = widgetFn();
            if (widget) emit showWidget(widget);
        }
    });

    this->setHeaderHidden(true);

    connect(AccountsManager::instance(), &AccountsManager::accountsChanged, this, &RepositoryBrowserList::updateData);
}

RepositoryBrowserList::~RepositoryBrowserList() {
    delete d;
}

void RepositoryBrowserList::addContextMenuFunction(QStandardItem* item, ContextMenuFunction fn) {
    item->setData(QVariant::fromValue(ContextMenuFunction(fn)),
        static_cast<int>(Roles::ContextMenuFunction));
}

void RepositoryBrowserList::addWidgetFunction(QStandardItem* item, WidgetFunction fn) {
    item->setData(QVariant::fromValue(WidgetFunction(fn)),
        static_cast<int>(Roles::WidgetFunction));
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
        addContextMenuFunction(item, [branch, this](QMenu* menu) {
            BranchUiHelper::appendBranchMenu(menu, branch, d->repo, this);
        });
        addWidgetFunction(item, [branch, this] {
            auto commitBrowser = new CommitBrowserWidget();
            commitBrowser->setRepository(d->repo);
            commitBrowser->setStartBranch(branch);
            return commitBrowser;
        });
        item->setData(QVariant::fromValue(branch.staticCast<QObject>()),
            static_cast<int>(Roles::Data));
        d->branchParent->appendRow(item);
    }

    d->remoteParent->removeRows(0, d->remoteParent->rowCount());
    for (auto remote : d->remotes) {
        auto item = new QStandardItem(remote->name());
        addContextMenuFunction(item, [remote, this](QMenu* menu) {
            BranchUiHelper::appendRemoteMenu(menu, remote, d->repo, this);
        });
        item->setData(QVariant::fromValue(remote.staticCast<QObject>()),
            static_cast<int>(Roles::Data));
        d->remoteParent->appendRow(item);

        for (auto remoteBranch : d->remoteBranches) {
            if (remoteBranch->remoteName() == remote->name()) {
                auto branchItem = new QStandardItem(remoteBranch->name());
                addContextMenuFunction(branchItem, [remoteBranch, this](QMenu* menu) {
                    BranchUiHelper::appendBranchMenu(menu, remoteBranch, d->repo, this);
                });
                addWidgetFunction(branchItem, [remoteBranch, this] {
                    auto commitBrowser = new CommitBrowserWidget();
                    commitBrowser->setRepository(d->repo);
                    commitBrowser->setStartBranch(remoteBranch);
                    return commitBrowser;
                });
                branchItem->setData(QVariant::fromValue(remoteBranch.staticCast<QObject>()),
                    static_cast<int>(Roles::Data));
                item->appendRow(branchItem);
            }
        }
    }

    d->stashParent->removeRows(0, d->stashParent->rowCount());
    for (auto stash : d->stashes) {
        auto item = new QStandardItem(stash->message());
        addContextMenuFunction(item, [stash, this](QMenu* menu) {
            BranchUiHelper::appendStashMenu(menu, stash, d->repo, this);
        });
        item->setData(QVariant::fromValue(stash.staticCast<QObject>()),
            static_cast<int>(Roles::Data));
        d->stashParent->appendRow(item);
    }

    for (auto controller : d->ghIssueControllers) controller->deleteLater();
    for (auto controller : d->ghPrControllers) controller->deleteLater();
    for (auto account : AccountsManager::instance()->accounts()) {
        if (auto gh = qobject_cast<GitHubAccount*>(account)) {
            for (auto remote : d->remotes) {
                auto slug = remote->slugForAccount(gh);
                if (slug.isEmpty()) continue;

                auto issuesController = new GitHubIssueListController(gh, remote);
                connect(issuesController, &GitHubIssueListController::destroyed, this, [issuesController, this] {
                    d->ghIssueControllers.removeAll(issuesController);
                });
                d->issuesParent->appendRow(issuesController->rootItem());
                d->ghIssueControllers.append(issuesController);

                auto prController = new GitHubPullRequestListController(gh, remote);
                connect(prController, &GitHubPullRequestListController::destroyed, this, [prController, this] {
                    d->ghPrControllers.removeAll(prController);
                });
                d->prsParent->appendRow(prController->rootItem());
                d->ghPrControllers.append(prController);
            }
        }
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
    auto menuFn = item->data(static_cast<int>(Roles::ContextMenuFunction)).value<ContextMenuFunction>();
    if (menuFn) menuFn(menu);

    connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
    menu->popup(this->mapToGlobal(pos));
}

void RepositoryBrowserList::contextMenuEvent(QContextMenuEvent* event) {
    this->showContextMenu(event->pos());
}
