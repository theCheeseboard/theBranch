#include "githubissuecommentevent.h"

#include "../githubaccount.h"
#include "../githubitemdatabase.h"
#include "../users/githubuser.h"
#include <QJsonObject>

struct GitHubIssueCommentEventPrivate {
        QString body;
        GitHubUserPtr user;
};

GitHubIssueCommentEvent::GitHubIssueCommentEvent(GitHubAccount* account, RemotePtr remote) :
    GitHubIssueEvent{account, remote} {
    d = new GitHubIssueCommentEventPrivate();
}

GitHubIssueCommentEvent::~GitHubIssueCommentEvent() {
    delete d;
}

QString GitHubIssueCommentEvent::body() {
    return d->body;
}

GitHubUserPtr GitHubIssueCommentEvent::user() {
    return d->user;
}

void GitHubIssueCommentEvent::update(QJsonObject data) {
    d->body = data.value("body").toString();
    d->user = this->account()->itemDb()->update<GitHubUser>(this->account(), this->remote(), data.value("user").toObject());

    GitHubIssueEvent::update(data);
}
