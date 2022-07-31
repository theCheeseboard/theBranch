#ifndef GITHUBISSUESAPI_H
#define GITHUBISSUESAPI_H

#include "objects/forward_declares.h"
#include <QCoroAsyncGenerator>

class GitHubIssue;

typedef QSharedPointer<GitHubIssue> GitHubIssuePtr;

class GitHubHttp;
class GitHubIssuesApi {
        Q_DECLARE_TR_FUNCTIONS(GitHubPullRequestApi)
    public:
        explicit GitHubIssuesApi(GitHubHttp* http);

        QCoro::AsyncGenerator<GitHubIssuePtr> listIssues(RemotePtr remote, QString state = "all");
        QCoro::Task<GitHubIssuePtr> issue(RemotePtr remote, qint64 issueNumber);

    private:
        GitHubHttp* http;
};

#endif // GITHUBISSUESAPI_H
