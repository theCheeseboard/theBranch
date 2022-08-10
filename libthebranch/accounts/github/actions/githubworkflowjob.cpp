#include "githubworkflowjob.h"

#include "../githubaccount.h"
#include "githubactionsapi.h"
#include <QJsonObject>

struct GitHubWorkflowJobPrivate {
        qint64 id;
        QString name;
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

void GitHubWorkflowJob::update(QJsonObject data) {
    d->id = data.value("id").toInteger();
    d->name = data.value("name").toString();

    GitHubItem::update(data);
}

QCoro::Task<> GitHubWorkflowJob::fetchLatest() {
    co_await this->account()->actions()->workflow(this->remote(), d->id);
}
