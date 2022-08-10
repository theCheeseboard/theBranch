#ifndef GITHUBWORKFLOWRUN_H
#define GITHUBWORKFLOWRUN_H

#include "../githubitem.h"
#include <QCoroAsyncGenerator>

class GitHubWorkflowJob;
typedef QSharedPointer<GitHubWorkflowJob> GitHubWorkflowJobPtr;

struct GitHubWorkflowRunPrivate;
class GitHubWorkflowRun : public GitHubItem {
        Q_OBJECT
    public:
        GitHubWorkflowRun(GitHubAccount* account, RemotePtr remote);
        ~GitHubWorkflowRun();

        QString name();
        qint64 runNumber();

        QString headCommitMessage();

        QCoro::AsyncGenerator<GitHubWorkflowJobPtr> listWorkflowJobs();

    private:
        GitHubWorkflowRunPrivate* d;

        // GitHubItem interface
    public:
        void update(QJsonObject data);
        QCoro::Task<> fetchLatest();
};

typedef QSharedPointer<GitHubWorkflowRun> GitHubWorkflowRunPtr;

#endif // GITHUBWORKFLOWRUN_H
