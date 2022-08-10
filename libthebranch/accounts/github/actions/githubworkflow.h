#ifndef GITHUBWORKFLOW_H
#define GITHUBWORKFLOW_H

#include "../githubitem.h"

struct GitHubWorkflowPrivate;
class GitHubWorkflow : public GitHubItem {
        Q_OBJECT
    public:
        GitHubWorkflow(GitHubAccount* account, RemotePtr remote);
        ~GitHubWorkflow();

        QString name();

    private:
        GitHubWorkflowPrivate* d;

        // GitHubItem interface
    public:
        void update(QJsonObject data);
        QCoro::Task<> fetchLatest();
};

typedef QSharedPointer<GitHubWorkflow> GitHubWorkflowPtr;

#endif // GITHUBWORKFLOW_H
