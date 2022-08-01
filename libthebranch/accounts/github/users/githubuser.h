#ifndef GITHUBUSER_H
#define GITHUBUSER_H

#include "../githubitem.h"
#include <QObject>

struct GitHubUserPrivate;
class GitHubUser : public GitHubItem {
        Q_OBJECT
    public:
        explicit GitHubUser(GitHubAccount* account, RemotePtr remote);
        ~GitHubUser();

        QString login();

    signals:

    private:
        GitHubUserPrivate* d;

        // GitHubItem interface
    public:
        void update(QJsonObject data);
        QCoro::Task<> fetchLatest();
};

typedef QSharedPointer<GitHubUser> GitHubUserPtr;

#endif // GITHUBUSER_H
