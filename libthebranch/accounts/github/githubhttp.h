#ifndef GITHUBHTTP_H
#define GITHUBHTTP_H

#include <QObject>

class GitHubAccount;
class QNetworkReply;
struct GitHubHttpPrivate;
class GitHubHttp : public QObject {
        Q_OBJECT
    public:
        explicit GitHubHttp(GitHubAccount* account, QObject* parent = nullptr);
        ~GitHubHttp();

        GitHubAccount* account();

        QNetworkReply* get(QString path);
        QNetworkReply* post(QString path, QByteArray payload);

    signals:

    private:
        GitHubHttpPrivate* d;
};

#endif // GITHUBHTTP_H
