#ifndef GITHUBISSUEEVENT_H
#define GITHUBISSUEEVENT_H

#include "../githubitem.h"

class GitHubUser;
typedef QSharedPointer<GitHubUser> GitHubUserPtr;

struct GitHubIssueEventPrivate;
class GitHubIssueEvent : public GitHubItem {
        Q_OBJECT
    public:
        explicit GitHubIssueEvent(GitHubAccount* account, RemotePtr remote);
        ~GitHubIssueEvent();

        QString event();
        QVariant eventProperty(QString property);
        GitHubUserPtr actor();

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
