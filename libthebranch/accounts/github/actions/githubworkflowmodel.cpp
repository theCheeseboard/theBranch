#include "githubworkflowmodel.h"

#include "githubworkflow.h"

struct GitHubWorkflowModelPrivate {
        GitHubWorkflowPtr workflow;
};

GitHubWorkflowModel::GitHubWorkflowModel(GitHubWorkflowPtr workflow, QObject* parent) :
    QAbstractListModel(parent) {
    d = new GitHubWorkflowModelPrivate();
    d->workflow = workflow;
}

GitHubWorkflowModel::~GitHubWorkflowModel() {
    delete d;
}

QVariant GitHubWorkflowModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();

    auto workflowRun = item(index);
    switch (role) {
        case Qt::DisplayRole:
            return QStringLiteral("%1: %2").arg(workflowRun->runNumber()).arg(workflowRun->headCommitMessage());
        case WorkflowRunRole:
            return QVariant::fromValue(workflowRun);
    }
    return QVariant();
}

QCoro::AsyncGenerator<GitHubWorkflowRunPtr> GitHubWorkflowModel::createGenerator() {
    return d->workflow->listWorkflowRuns();
}
