#include "githubworkflowrun.h"

#include "../githubaccount.h"
#include "githubactionsapi.h"
#include <QJsonObject>

struct GitHubWorkflowRunPrivate {
        qint64 id;
};

GitHubWorkflowRun::GitHubWorkflowRun(GitHubAccount* account, RemotePtr remote) :
    GitHubItem{account, remote} {
    d = new GitHubWorkflowRunPrivate();
}

GitHubWorkflowRun::~GitHubWorkflowRun() {
    delete d;
}

void GitHubWorkflowRun::update(QJsonObject data) {
    d->id = data.value("id").toInteger();
    GitHubItem::update(data);
}

QCoro::Task<> GitHubWorkflowRun::fetchLatest() {
    co_await this->account()->actions()->workflow(this->remote(), d->id);
}
