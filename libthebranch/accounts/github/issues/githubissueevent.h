#ifndef GITHUBISSUEEVENT_H
#define GITHUBISSUEEVENT_H

#include "../githubitem.h"

struct GitHubIssueEventPrivate;
class GitHubIssueEvent : public GitHubItem {
        Q_OBJECT
    public:
        explicit GitHubIssueEvent(GitHubAccount* account, RemotePtr remote);
        ~GitHubIssueEvent();

        QString event();

    signals:

    private:
        GitHubIssueEventPrivate* d;

        // GitHubItem interface
    public:
        void update(QJsonObject data);
        QCoro::Task<> fetchLatest();
};

typedef QSharedPointer<GitHubIssueEvent> GitHubIssueEventPtr;

#endif // GITHUBISSUEEVENT_H
