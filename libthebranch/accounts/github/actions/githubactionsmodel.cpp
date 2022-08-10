#include "githubactionsmodel.h"

#include "../githubaccount.h"
#include "githubactionsapi.h"
#include "githubworkflow.h"

struct GitHubActionsModelPrivate {
        GitHubAccount* account;
        RemotePtr remote;
        QList<GitHubWorkflowPtr> workflows;

        bool fetching = false;
        bool requireInitialFetch = true;
        QCoro::AsyncGenerator<GitHubWorkflowPtr> workflowGenerator;
        QCoro::AsyncGenerator<GitHubWorkflowPtr>::iterator workflowIterator = QCoro::AsyncGenerator<GitHubWorkflowPtr>::iterator(nullptr);
};

GitHubActionsModel::GitHubActionsModel(GitHubAccount* account, RemotePtr remote, QObject* parent) :
    QAbstractListModel(parent) {
    d = new GitHubActionsModelPrivate();
    d->account = account;
    d->remote = remote;
}

GitHubActionsModel::~GitHubActionsModel() {
    delete d;
}

int GitHubActionsModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;

    return d->workflows.length();
}

bool GitHubActionsModel::hasChildren(const QModelIndex& parent) const {
    return false;
}

bool GitHubActionsModel::canFetchMore(const QModelIndex& parent) const {
    if (d->requireInitialFetch) return true;
    if (d->fetching) return true;
    return d->workflowIterator != d->workflowGenerator.end();
}

void GitHubActionsModel::fetchMore(const QModelIndex& parent) {
    this->fetchNext();
}

QVariant GitHubActionsModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();

    auto workflow = d->workflows.at(index.row());
    if (role == Qt::DisplayRole) {
        return workflow->name();
    }
    return QVariant();
}

QCoro::Task<> GitHubActionsModel::startFetch() {
    beginRemoveRows(QModelIndex(), 0, this->rowCount());
    d->workflows.clear();
    endRemoveRows();

    d->workflowGenerator = d->account->actions()->listWorkflows(d->remote);
    d->workflowIterator = co_await d->workflowGenerator.begin();
}

QCoro::Task<> GitHubActionsModel::fetchNext() {
    if (d->fetching) co_return;

    d->fetching = true;
    if (d->requireInitialFetch) {
        d->requireInitialFetch = false;
        co_await startFetch();
    }

    for (auto i = 0; i < 100; i++) {
        if (d->workflowIterator == d->workflowGenerator.end()) {
            d->fetching = false;
            co_return; // Nothing else to add
        }

        emit beginInsertRows(QModelIndex(), this->rowCount(), this->rowCount() + 1);
        d->workflows.append(*d->workflowIterator);
        emit endInsertRows();

        co_await ++(d->workflowIterator);
    }
    d->fetching = false;
}
