#include "githubpullrequest.h"

#include <QJsonObject>

struct GitHubPullRequestPrivate {
};

GitHubPullRequest::GitHubPullRequest(GitHubAccount* account, RemotePtr remote) :
    GitHubIssue{account, remote} {
    d = new GitHubPullRequestPrivate();
}

GitHubPullRequest::~GitHubPullRequest() {
    delete d;
}

void GitHubPullRequest::update(QJsonObject data) {
    GitHubIssue::update(data);
}
