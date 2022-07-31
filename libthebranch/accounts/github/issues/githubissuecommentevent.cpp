#include "githubissuecommentevent.h"

#include <QJsonObject>

struct GitHubIssueCommentEventPrivate {
        QString body;
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

void GitHubIssueCommentEvent::update(QJsonObject data) {
    d->body = data.value("body").toString();
    GitHubIssueEvent::update(data);
}
