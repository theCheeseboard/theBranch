#include "githubactionsmodel.h"

#include "../githubaccount.h"
#include "githubactionsapi.h"
#include "githubworkflow.h"

struct GitHubActionsModelPrivate {
        GitHubAccount* account;
        RemotePtr remote;
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

QVariant GitHubActionsModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();

    auto workflow = item(index);
    switch (role) {
        case Qt::DisplayRole:
            return workflow->name();
        case WorkflowRole:
            return QVariant::fromValue(workflow);
    }
    return QVariant();
}

QCoro::AsyncGenerator<GitHubWorkflowPtr> GitHubActionsModel::createGenerator() {
    return d->account->actions()->listWorkflows(d->remote);
}
