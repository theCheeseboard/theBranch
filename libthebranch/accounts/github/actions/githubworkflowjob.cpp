#include "githubworkflowjob.h"

#include "../githubaccount.h"
#include "githubactionsapi.h"
#include <QJsonArray>
#include <QJsonObject>

struct GitHubWorkflowJobPrivate {
        qint64 id;
        QString name;
        QList<GitHubWorkflowJob::JobStep> steps;
};

GitHubWorkflowJob::GitHubWorkflowJob(GitHubAccount* account, RemotePtr remote) :
    GitHubItem{account, remote} {
    d = new GitHubWorkflowJobPrivate();
}

GitHubWorkflowJob::~GitHubWorkflowJob() {
    delete d;
}

QString GitHubWorkflowJob::name() {
    return d->name;
}

QList<GitHubWorkflowJob::JobStep> GitHubWorkflowJob::steps() {
    return d->steps;
}

void GitHubWorkflowJob::update(QJsonObject data) {
    d->id = data.value("id").toInteger();
    d->name = data.value("name").toString();

    d->steps.clear();
    for (auto stepVal : data.value("steps").toArray()) {
        auto stepObj = stepVal.toObject();

        JobStep step;
        step.name = stepObj.value("name").toString();
        step.completed = QDateTime::fromString(stepObj.value("completed_at").toString());
        step.started = QDateTime::fromString(stepObj.value("started_at").toString());

        auto status = stepObj.value("status").toString();
        if (status == "completed") {
            step.status = JobStep::Status::Completed;
        } else if (status == "in_progress") {
            step.status = JobStep::Status::InProgress;
        } else if (status == "queued") {
            step.status = JobStep::Status::Queued;
        } else if (status == "pending") {
            step.status = JobStep::Status::Pending;
        }

        auto conclusion = stepObj.value("conclusion").toString();
        if (conclusion == "success") {
            step.conclusion = JobStep::Conclusion::Success;
        } else if (conclusion == "failure") {
            step.conclusion = JobStep::Conclusion::Failure;
        } else if (conclusion == "skipped") {
            step.conclusion = JobStep::Conclusion::Skipped;
        }

        d->steps.append(step);
    }

    GitHubItem::update(data);
}

QCoro::Task<> GitHubWorkflowJob::fetchLatest() {
    co_await this->account()->actions()->workflowJob(this->remote(), d->id);
}
