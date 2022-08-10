#ifndef GITHUBACCOUNT_H
#define GITHUBACCOUNT_H

#include "../abstractaccount.h"
#include <QCoroTask>

class GitHubItemDatabase;
class GitHubIssuesApi;
class GitHubPullRequestApi;
class GitHubActionsApi;
struct GitHubAccountPrivate;
class GitHubAccount : public AbstractAccount {
        Q_OBJECT
    public:
        explicit GitHubAccount(QString username, QString token, QObject* parent = nullptr);
        explicit GitHubAccount(QJsonObject account, QObject* parent = nullptr);
        ~GitHubAccount();

        QString username();
        QString token();
        QString description();
        QString slugForCloneUrl(QString cloneUrl);

        GitHubItemDatabase* itemDb();

        GitHubPullRequestApi* pr();
        GitHubIssuesApi* issues();
        GitHubActionsApi* actions();

        QCoro::Task<bool> testConnection();

    signals:

    private:
        GitHubAccountPrivate* d;
        void init();

        // AbstractAccount interface
    public:
        QJsonObject save();
};

#endif // GITHUBACCOUNT_H
