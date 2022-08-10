#ifndef GITHUBWORKFLOW_H
#define GITHUBWORKFLOW_H

#include "../githubitem.h"
#include "objects/forward_declares.h"
#include <QCoroAsyncGenerator>

class GitHubWorkflowRun;
typedef QSharedPointer<GitHubWorkflowRun> GitHubWorkflowRunPtr;

struct GitHubWorkflowPrivate;
class GitHubWorkflow : public GitHubItem,
                       public tbSharedFromThis<GitHubWorkflow> {
        Q_OBJECT
    public:
        GitHubWorkflow(GitHubAccount* account, RemotePtr remote);
        ~GitHubWorkflow();

        QString name();

        QCoro::AsyncGenerator<GitHubWorkflowRunPtr> listWorkflowRuns();

    private:
        GitHubWorkflowPrivate* d;

        // GitHubItem interface
    public:
        void update(QJsonObject data);
        QCoro::Task<> fetchLatest();

        // GitHubItem interface
    public:
        QWidget* widget();
};

typedef QSharedPointer<GitHubWorkflow> GitHubWorkflowPtr;

#endif // GITHUBWORKFLOW_H
