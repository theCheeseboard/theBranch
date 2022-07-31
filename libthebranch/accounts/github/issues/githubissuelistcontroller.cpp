#include "githubissuelistcontroller.h"

#include "../githubaccount.h"
#include "githubissue.h"
#include "githubissuesapi.h"
#include "objects/remote.h"
#include <QStandardItem>
#include <QTimer>

struct GitHubIssueListControllerPrivate {
        QStandardItem* rootItem;
        GitHubAccount* account;
        RemotePtr remote;

        QTimer* timer;
};

GitHubIssueListController::GitHubIssueListController(GitHubAccount* account, RemotePtr remote, QObject* parent) :
    QObject{parent} {
    d = new GitHubIssueListControllerPrivate();
    d->rootItem = new QStandardItem(remote->name());
    d->account = account;
    d->remote = remote;

    d->timer = new QTimer(this);
    d->timer->setInterval(30000);
    connect(d->timer, &QTimer::timeout, this, &GitHubIssueListController::updateItems);
    d->timer->start();

    updateItems();
}

GitHubIssueListController::~GitHubIssueListController() {
    d->rootItem->parent()->removeRow(d->rootItem->row());
    delete d;
}

QStandardItem* GitHubIssueListController::rootItem() {
    return d->rootItem;
}

QCoro::Task<> GitHubIssueListController::updateItems() {
    QList<QStandardItem*> items;
    QCORO_FOREACH(auto issue, d->account->issues()->listIssues(d->remote, "open")) {
        auto item = new QStandardItem(QStringLiteral("%1: %2").arg(issue->number()).arg(issue->title()));
        item->setData(QVariant::fromValue(issue->nodeId()), Qt::UserRole);
        items.append(item);
    }

    d->rootItem->removeRows(0, d->rootItem->rowCount());
    d->rootItem->appendRows(items);
    co_return;
}
