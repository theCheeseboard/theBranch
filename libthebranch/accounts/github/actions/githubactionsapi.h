#ifndef GITHUBACTIONSAPI_H
#define GITHUBACTIONSAPI_H

#include "objects/forward_declares.h"
#include <QCoroAsyncGenerator>
#include <QObject>

class GitHubWorkflow;
class GitHubWorkflowRun;

typedef QSharedPointer<GitHubWorkflow> GitHubWorkflowPtr;
typedef QSharedPointer<GitHubWorkflowRun> GitHubWorkflowRunPtr;

class GitHubHttp;
class GitHubActionsApi {
        Q_DECLARE_TR_FUNCTIONS(GitHubActionsApi)
    public:
        GitHubActionsApi(GitHubHttp* http);

        QCoro::AsyncGenerator<GitHubWorkflowPtr> listWorkflows(RemotePtr remote);
        QCoro::Task<GitHubWorkflowPtr> workflow(RemotePtr remote, qint64 workflowId);

        QCoro::AsyncGenerator<GitHubWorkflowRunPtr> listWorkflowRuns(RemotePtr remote, qint64 workflowId);
        QCoro::Task<GitHubWorkflowRunPtr> workflowRun(RemotePtr remote, qint64 workflowRunId);

    private:
        GitHubHttp* http;
};

#endif // GITHUBACTIONSAPI_H
