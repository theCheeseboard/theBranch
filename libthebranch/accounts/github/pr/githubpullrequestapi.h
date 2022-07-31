#ifndef GITHUBPULLREQUESTAPI_H
#define GITHUBPULLREQUESTAPI_H

#include "objects/forward_declares.h"
#include <QCoroAsyncGenerator>
#include <QCoroTask>

class GitHubIssue;

typedef QSharedPointer<GitHubIssue> GitHubIssuePtr;

class GitHubHttp;
class GitHubPullRequestApi {
        Q_DECLARE_TR_FUNCTIONS(GitHubPullRequestApi)
    public:
        GitHubPullRequestApi(GitHubHttp* http);

        QCoro::Task<> createPullRequest(RemotePtr remote, BranchPtr from, BranchPtr to, QString title, QString body);
        QCoro::AsyncGenerator<GitHubIssuePtr> listPullRequests(RemotePtr remote, QString state = "all");

    private:
        GitHubHttp* http;
};

#endif // GITHUBPULLREQUESTAPI_H
