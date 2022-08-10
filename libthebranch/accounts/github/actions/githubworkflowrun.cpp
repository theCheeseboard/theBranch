#include "githubworkflowrun.h"

#include "../githubaccount.h"
#include "githubactionsapi.h"
#include <QJsonObject>

struct GitHubWorkflowRunPrivate {
        qint64 id;
        qint64 number;
        QString name;
        QString headCommitMessage;
};

GitHubWorkflowRun::GitHubWorkflowRun(GitHubAccount* account, RemotePtr remote) :
    GitHubItem{account, remote} {
    d = new GitHubWorkflowRunPrivate();
}

GitHubWorkflowRun::~GitHubWorkflowRun() {
    delete d;
}

QString GitHubWorkflowRun::name() {
    return d->name;
}

qint64 GitHubWorkflowRun::runNumber() {
    return d->number;
}

QString GitHubWorkflowRun::headCommitMessage() {
    return d->headCommitMessage;
}

void GitHubWorkflowRun::update(QJsonObject data) {
    d->id = data.value("id").toInteger();
    d->name = data.value("name").toString();
    d->number = data.value("run_number").toInteger();

    auto headCommit = data.value("head_commit").toObject();
    d->headCommitMessage = headCommit.value("message").toString();

    GitHubItem::update(data);
}

QCoro::Task<> GitHubWorkflowRun::fetchLatest() {
    co_await this->account()->actions()->workflow(this->remote(), d->id);
}
