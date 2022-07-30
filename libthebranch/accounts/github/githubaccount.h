#ifndef GITHUBACCOUNT_H
#define GITHUBACCOUNT_H

#include "../abstractaccount.h"
#include <QCoroTask>

class GitHubPullRequestApi;
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

        GitHubPullRequestApi* pr();

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
