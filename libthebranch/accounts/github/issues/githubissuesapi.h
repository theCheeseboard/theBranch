#ifndef GITHUBISSUESAPI_H
#define GITHUBISSUESAPI_H

#include "objects/forward_declares.h"
#include <QCoroAsyncGenerator>

class GitHubIssue;
class GitHubIssueEvent;

typedef QSharedPointer<GitHubIssue> GitHubIssuePtr;
typedef QSharedPointer<GitHubIssueEvent> GitHubIssueEventPtr;

class GitHubHttp;
class GitHubIssuesApi {
        Q_DECLARE_TR_FUNCTIONS(GitHubPullRequestApi)
    public:
        explicit GitHubIssuesApi(GitHubHttp* http);

        QCoro::AsyncGenerator<GitHubIssuePtr> listIssues(RemotePtr remote, QString state = "all");
        QCoro::Task<GitHubIssuePtr> issue(RemotePtr remote, qint64 issueNumber);

        QCoro::AsyncGenerator<GitHubIssueEventPtr> listIssueEvents(RemotePtr remote, qint64 issueNumber);
        QCoro::Task<> updateIssue(RemotePtr remote, qint64 issueNumber, QJsonObject fieldsToUpdate);
        QCoro::Task<> postComment(RemotePtr remote, qint64 issueNumber, QString commentBody);

    private:
        GitHubHttp* http;
};

#endif // GITHUBISSUESAPI_H
