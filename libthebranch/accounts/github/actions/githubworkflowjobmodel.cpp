#include "githubworkflowjobmodel.h"

#include "githubworkflowjob.h"
#include <QIcon>

#include <QTimer>

struct GitHubWorkflowJobModelPrivate {
        GitHubWorkflowJobPtr workflowJob;
        QTimer* timer;
};

GitHubWorkflowJobModel::GitHubWorkflowJobModel(GitHubWorkflowJobPtr workflowJob, QObject* parent) :
    QAbstractListModel(parent) {
    d = new GitHubWorkflowJobModelPrivate();
    d->workflowJob = workflowJob;

    d->timer = new QTimer(this);
    d->timer->setInterval(10000);
    connect(d->timer, &QTimer::timeout, this, [this] {
        d->workflowJob->fetchLatest();
    });
    d->timer->start();

    connect(d->workflowJob.data(), &GitHubWorkflowJob::updated, this, [this] {
        emit dataChanged(this->index(0), this->index(rowCount()));
    });
}

GitHubWorkflowJobModel::~GitHubWorkflowJobModel() {
    delete d;
}

int GitHubWorkflowJobModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return d->workflowJob->steps().count();
}

QVariant GitHubWorkflowJobModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (d->workflowJob->steps().count() <= index.row()) return QVariant();
    auto step = d->workflowJob->steps().at(index.row());
    switch (role) {
        case Qt::DisplayRole:
            return step.name;
        case Qt::DecorationRole:
            switch (step.status) {
                case GitHubWorkflowJob::JobStep::Status::Completed:
                    switch (step.conclusion) {
                        case GitHubWorkflowJob::JobStep::Conclusion::Success:
                            return QIcon::fromTheme("dialog-ok");
                        case GitHubWorkflowJob::JobStep::Conclusion::Failure:
                            return QIcon::fromTheme("dialog-cancel");
                        case GitHubWorkflowJob::JobStep::Conclusion::Skipped:
                            return QIcon::fromTheme("edit-redo");
                    }
                case GitHubWorkflowJob::JobStep::Status::InProgress:
                case GitHubWorkflowJob::JobStep::Status::Pending:
                    return QIcon::fromTheme("media-playback-start");
                case GitHubWorkflowJob::JobStep::Status::Queued:
                    return QIcon::fromTheme("media-playback-stop");
            }
    }

    return QVariant();
}
