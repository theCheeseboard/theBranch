#include "githubworkflowrunmodel.h"

#include "githubworkflowrun.h"

struct GitHubWorkflowRunModelPrivate {
        GitHubWorkflowRunPtr workflowRun;
};

GitHubWorkflowRunModel::GitHubWorkflowRunModel(GitHubWorkflowRunPtr workflowRun, QObject* parent) :
    QAbstractListModel(parent) {
    d = new GitHubWorkflowRunModelPrivate();
    d->workflowRun = workflowRun;
}

GitHubWorkflowRunModel::~GitHubWorkflowRunModel() {
    delete d;
}

QVariant GitHubWorkflowRunModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();

    auto job = item(index);
    switch (role) {
        case Qt::DisplayRole:
            return job->name();
    }

    return QVariant();
}

QCoro::AsyncGenerator<GitHubWorkflowJobPtr> GitHubWorkflowRunModel::createGenerator() {
    return d->workflowRun->listWorkflowJobs();
}
