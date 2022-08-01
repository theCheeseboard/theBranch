#include "githubissue.h"

#include "../githubaccount.h"
#include "../githubitemdatabase.h"
#include "../pr/githubpullrequest.h"
#include "../users/githubuser.h"
#include "githubissuesapi.h"
#include <QDesktopServices>
#include <QJsonObject>
#include <QMenu>

struct GitHubIssuePrivate {
        qint64 number;
        QString title;
        QString body;
        GitHubIssue::State state;
        GitHubUserPtr user;
};

GitHubIssue::GitHubIssue(GitHubAccount* account, RemotePtr remote) :
    GitHubItem{account, remote} {
    d = new GitHubIssuePrivate();
}

GitHubIssue::~GitHubIssue() {
    delete d;
}

qint64 GitHubIssue::number() {
    return d->number;
}

QString GitHubIssue::title() {
    return d->title;
}

QString GitHubIssue::body() {
    return d->body;
}

GitHubIssue::State GitHubIssue::state() {
    return d->state;
}

GitHubUserPtr GitHubIssue::user() {
    return d->user;
}

QCoro::AsyncGenerator<GitHubIssueEventPtr> GitHubIssue::listIssueEvents() {
    return this->account()->issues()->listIssueEvents(this->remote(), d->number);
}

QCoro::Task<> GitHubIssue::postComment(QString comment) {
    return this->account()->issues()->postComment(this->remote(), d->number, comment);
}

QCoro::Task<> GitHubIssue::setState(State state) {
    if (state == State::Merged) co_return; // This doesn't make sense
    QJsonObject fieldsToUpdate = {
        {"state", state == State::Open ? "open" : "closed"}
    };
    co_await this->account()->issues()->updateIssue(this->remote(), d->number, fieldsToUpdate);
}

QCoro::Task<> GitHubIssue::fetchLatest() {
    co_await this->account()->issues()->issue(this->remote(), d->number);
    co_return;
}

void GitHubIssue::update(QJsonObject data) {
    d->number = data.value("number").toInteger();
    d->title = data.value("title").toString();
    d->body = data.value("body").toString();

    QString stateString = data.value("state").toString();
    if (stateString == "open") {
        d->state = State::Open;
    } else if (stateString == "closed") {
        d->state = State::Closed;
    }

    d->user = this->account()->itemDb()->update<GitHubUser>(this->account(), this->remote(), data.value("user").toObject());
    GitHubItem::update(data);
}

void GitHubIssue::contextMenu(QMenu* menu) {
    if (qobject_cast<GitHubPullRequest*>(this)) {
        menu->addSection(tr("For Pull Request #%1").arg(d->number));
    } else {
        menu->addSection(tr("For Issue #%1").arg(d->number));
    }
    menu->addAction(tr("View on GitHub"), [this] {
        QDesktopServices::openUrl(this->htmlUrl());
    });
}
