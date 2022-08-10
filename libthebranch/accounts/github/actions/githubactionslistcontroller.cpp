#include "githubactionslistcontroller.h"

#include "../githubaccount.h"
#include "githubactionsapi.h"
#include "githubactionsbrowser.h"
#include "githubworkflow.h"
#include "objects/remote.h"
#include "widgets/repositorybrowserlist.h"
#include <QPointer>
#include <QStandardItem>
#include <QTimer>

struct GitHubActionsListControllerPrivate {
        QStandardItem* rootItem;
        GitHubAccount* account;
        RemotePtr remote;

        QTimer* timer;
};

GitHubActionsListController::GitHubActionsListController(GitHubAccount* account, RemotePtr remote, QObject* parent) :
    QObject{parent} {
    d = new GitHubActionsListControllerPrivate();
    d->rootItem = new QStandardItem(remote->name());
    d->account = account;
    d->remote = remote;

    d->timer = new QTimer(this);
    d->timer->setInterval(30000);
    connect(d->timer, &QTimer::timeout, this, &GitHubActionsListController::updateItems);
    d->timer->start();

    RepositoryBrowserList::addWidgetFunction(d->rootItem, [account, remote] {
        return new GitHubActionsBrowser(account, remote);
    });

    updateItems();
}

GitHubActionsListController::~GitHubActionsListController() {
    d->rootItem->parent()->removeRow(d->rootItem->row());
    delete d;
}

QStandardItem* GitHubActionsListController::rootItem() {
    return d->rootItem;
}

QCoro::Task<> GitHubActionsListController::updateItems() {
    QPointer<GitHubActionsListController> thisPtr = this;
    QList<QStandardItem*> items;
    QCORO_FOREACH(auto workflow, d->account->actions()->listWorkflows(d->remote)) {
        auto item = new QStandardItem(workflow->name());
        RepositoryBrowserList::addContextMenuFunction(item, [workflow](QMenu* menu) {
            workflow->contextMenu(menu);
        });
        RepositoryBrowserList::addWidgetFunction(item, [workflow] {
            return workflow->widget();
        });
        items.append(item);
    }

    if (!thisPtr) co_return;
    d->rootItem->removeRows(0, d->rootItem->rowCount());
    d->rootItem->appendRows(items);
    co_return;
}
