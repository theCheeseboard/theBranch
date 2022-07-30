#ifndef GITHUBPULLREQUESTAPI_H
#define GITHUBPULLREQUESTAPI_H

#include "objects/forward_declares.h"
#include <QCoroAsyncGenerator>
#include <QCoroTask>

class GitHubHttp;
class GitHubPullRequestApi {
        Q_DECLARE_TR_FUNCTIONS(GitHubPullRequestApi)
    public:
        GitHubPullRequestApi(GitHubHttp* http);

        struct PullRequest {
                PullRequest(QJsonObject def);

                quint64 id;
                quint64 number;
                QString title;
                QString body;
        };

        QCoro::Task<> createPullRequest(RemotePtr remote, BranchPtr from, BranchPtr to, QString title, QString body);
        QCoro::AsyncGenerator<PullRequest> listPullRequests(RemotePtr remote, QString state = "all");

    private:
        GitHubHttp* http;
};

#endif // GITHUBPULLREQUESTAPI_H
