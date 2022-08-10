#ifndef GITHUBWORKFLOWRUN_H
#define GITHUBWORKFLOWRUN_H

#include "../githubitem.h"

struct GitHubWorkflowRunPrivate;
class GitHubWorkflowRun : public GitHubItem {
        Q_OBJECT
    public:
        GitHubWorkflowRun(GitHubAccount* account, RemotePtr remote);
        ~GitHubWorkflowRun();

    private:
        GitHubWorkflowRunPrivate* d;

        // GitHubItem interface
    public:
        void update(QJsonObject data);
        QCoro::Task<> fetchLatest();
};

#endif // GITHUBWORKFLOWRUN_H
