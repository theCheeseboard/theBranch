#include "githubpullrequestlistcontroller.h"

#include "../githubaccount.h"
#include "../issues/githubissuebrowser.h"
#include "githubpullrequest.h"
#include "githubpullrequestapi.h"
#include "objects/remote.h"
#include "widgets/repositorybrowserlist.h"
#include <QPointer>
#include <QStandardItem>
#include <QTimer>

struct GitHubPullRequestListControllerPrivate {
        QStandardItem* rootItem;
        GitHubAccount* account;
        RemotePtr remote;

        QTimer* timer;
};

GitHubPullRequestListController::GitHubPullRequestListController(GitHubAccount* account, RemotePtr remote, QObject* parent) :
    QObject{parent} {
    d = new GitHubPullRequestListControllerPrivate();
    d->rootItem = new QStandardItem(remote->name());
    d->account = account;
    d->remote = remote;

    d->timer = new QTimer(this);
    d->timer->setInterval(30000);
    connect(d->timer, &QTimer::timeout, this, &GitHubPullRequestListController::updateItems);
    d->timer->start();

    RepositoryBrowserList::addWidgetFunction(d->rootItem, [account, remote] {
        return new GitHubIssueBrowser(account, remote, true);
    });

    updateItems();
}

GitHubPullRequestListController::~GitHubPullRequestListController() {
    d->rootItem->parent()->removeRow(d->rootItem->row());
    delete d;
}

QStandardItem* GitHubPullRequestListController::rootItem() {
    return d->rootItem;
}

QCoro::Task<> GitHubPullRequestListController::updateItems() {
    QPointer<GitHubPullRequestListController> thisPtr = this;
    QList<QStandardItem*> items;
    QCORO_FOREACH(auto issue, d->account->pr()->listPullRequests(d->remote, "open")) {
        auto pr = issue.objectCast<GitHubPullRequest>();
        auto item = new QStandardItem(QStringLiteral("%1: %2").arg(pr->number()).arg(pr->title()));
        RepositoryBrowserList::addContextMenuFunction(item, [pr](QMenu* menu) {
            pr->contextMenu(menu);
        });
        RepositoryBrowserList::addWidgetFunction(item, [pr] {
            return pr->widget();
        });
        items.append(item);
    }

    if (!thisPtr) co_return;
    d->rootItem->removeRows(0, d->rootItem->rowCount());
    d->rootItem->appendRows(items);
}
