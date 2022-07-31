#ifndef GITHUBPULLREQUEST_H
#define GITHUBPULLREQUEST_H

#include "../issues/githubissue.h"

struct GitHubPullRequestPrivate;
class GitHubPullRequest : public GitHubIssue {
        Q_OBJECT
    public:
        explicit GitHubPullRequest(GitHubAccount* account, RemotePtr remote);
        ~GitHubPullRequest();

    signals:

    private:
        GitHubPullRequestPrivate* d;

        // GitHubItem interface
    public:
        void update(QJsonObject data);
};

typedef QSharedPointer<GitHubPullRequest> GitHubPullRequestPtr;

#endif // GITHUBPULLREQUEST_H
