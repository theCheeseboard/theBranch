#include "githubpullrequestlistcontroller.h"

#include "../githubaccount.h"
#include "githubpullrequestapi.h"
#include "objects/remote.h"
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
    QList<QStandardItem*> items;
    QCORO_FOREACH(auto pr, d->account->pr()->listPullRequests(d->remote, "open")) {
        auto item = new QStandardItem(QStringLiteral("%1: %2").arg(pr.number).arg(pr.title));
        item->setData(QVariant::fromValue(pr), Qt::UserRole);
        items.append(item);
    }

    d->rootItem->removeRows(0, d->rootItem->rowCount());
    d->rootItem->appendRows(items);
}
