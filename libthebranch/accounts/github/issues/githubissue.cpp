#include "githubissue.h"

#include "../githubaccount.h"
#include "../pr/githubpullrequest.h"
#include "githubissuesapi.h"
#include <QDesktopServices>
#include <QJsonObject>
#include <QMenu>

struct GitHubIssuePrivate {
        qint64 number;
        QString title;
        QString body;
        GitHubIssue::State state;
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

QCoro::Task<> GitHubIssue::fetchLatest() {
    this->account()->issues()->issue(this->remote(), d->number);
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
