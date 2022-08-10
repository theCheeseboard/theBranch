#ifndef GITHUBWORKFLOWJOB_H
#define GITHUBWORKFLOWJOB_H

#include "../githubitem.h"
#include <QDateTime>

struct GitHubWorkflowJobPrivate;
class GitHubWorkflowJob : public GitHubItem {
        Q_OBJECT
    public:
        GitHubWorkflowJob(GitHubAccount* account, RemotePtr remote);
        ~GitHubWorkflowJob();

        struct JobStep {
                enum class Status {
                    Completed,
                    InProgress,
                    Queued,
                    Pending
                };

                enum class Conclusion {
                    Success,
                    Failure,
                    Skipped
                };

                QString name;
                QDateTime started;
                QDateTime completed;
                Status status;
                Conclusion conclusion;
        };

        QString name();

        QList<JobStep> steps();

    private:
        GitHubWorkflowJobPrivate* d;

        // GitHubItem interface
    public:
        void update(QJsonObject data);
        QCoro::Task<> fetchLatest();
};

typedef QSharedPointer<GitHubWorkflowJob> GitHubWorkflowJobPtr;

#endif // GITHUBWORKFLOWJOB_H
