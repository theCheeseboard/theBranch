#ifndef GITHUBWORKFLOWJOB_H
#define GITHUBWORKFLOWJOB_H

#include "../githubitem.h"

struct GitHubWorkflowJobPrivate;
class GitHubWorkflowJob : public GitHubItem {
        Q_OBJECT
    public:
        GitHubWorkflowJob(GitHubAccount* account, RemotePtr remote);
        ~GitHubWorkflowJob();

        QString name();

    private:
        GitHubWorkflowJobPrivate* d;

        // GitHubItem interface
    public:
        void update(QJsonObject data);
        QCoro::Task<> fetchLatest();
};

typedef QSharedPointer<GitHubWorkflowJob> GitHubWorkflowJobPtr;

#endif // GITHUBWORKFLOWJOB_H
