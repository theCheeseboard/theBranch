#include "githubpullrequest.h"

#include <QJsonObject>

struct GitHubPullRequestPrivate {
};

GitHubPullRequest::GitHubPullRequest() :
    GitHubIssue{} {
    d = new GitHubPullRequestPrivate();
}

GitHubPullRequest::~GitHubPullRequest() {
    delete d;
}

void GitHubPullRequest::update(QJsonObject data) {
    GitHubIssue::update(data);
}
