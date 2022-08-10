#include "githubworkflow.h"

#include "../githubaccount.h"
#include "githubactionsapi.h"
#include "githubactionsbrowser.h"
#include "objects/forward_declares.h"

struct GitHubWorkflowPrivate {
        QString name;
        qint64 id;
};

GitHubWorkflow::GitHubWorkflow(GitHubAccount* account, RemotePtr remote) :
    GitHubItem{account, remote} {
    d = new GitHubWorkflowPrivate();
}

GitHubWorkflow::~GitHubWorkflow() {
    delete d;
}

QString GitHubWorkflow::name() {
    return d->name;
}

QCoro::AsyncGenerator<GitHubWorkflowRunPtr> GitHubWorkflow::listWorkflowRuns() {
    return this->account()->actions()->listWorkflowRuns(this->remote(), d->id);
}

void GitHubWorkflow::update(QJsonObject data) {
    d->name = data.value("name").toString();
    d->id = data.value("id").toInteger();
    GitHubItem::update(data);
}

QCoro::Task<> GitHubWorkflow::fetchLatest() {
    co_await this->account()->actions()->workflow(this->remote(), d->id);
}

QWidget* GitHubWorkflow::widget() {
    auto browser = new GitHubActionsBrowser(this->account(), this->remote());
    browser->showWorkflow(this->sharedFromThis());
    return browser;
}
