#ifndef GITHUBPULLREQUESTAPI_H
#define GITHUBPULLREQUESTAPI_H

#include "objects/forward_declares.h"
#include <QCoroTask>

class GitHubHttp;
class GitHubPullRequestApi {
        Q_DECLARE_TR_FUNCTIONS(GitHubPullRequestApi)
    public:
        GitHubPullRequestApi(GitHubHttp* http);

        QCoro::Task<QString> createPullRequest(RemotePtr remote, BranchPtr from, BranchPtr to, QString title, QString body);

    private:
        GitHubHttp* http;
};

#endif // GITHUBPULLREQUESTAPI_H
