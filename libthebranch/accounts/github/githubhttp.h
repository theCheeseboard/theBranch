#ifndef GITHUBHTTP_H
#define GITHUBHTTP_H

#include <QException>
#include <QObject>
#include <QUrlQuery>

class GitHubException : public QException {
    public:
        GitHubException(QString error);
        GitHubException(QJsonObject response, QString defaultError);

        QString error() const;

        void raise() const override;
        GitHubException* clone() const override;

    private:
        QString _error;
};

class GitHubAccount;
class QNetworkReply;
struct GitHubHttpPrivate;
class GitHubHttp : public QObject {
        Q_OBJECT
    public:
        explicit GitHubHttp(GitHubAccount* account, QObject* parent = nullptr);
        ~GitHubHttp();

        GitHubAccount* account();

        QUrl makeUrl(QString path, QUrlQuery query = {});

        QNetworkReply* get(QString path);
        QNetworkReply* get(QUrl url);

        QNetworkReply* post(QString path, QByteArray payload);
        QNetworkReply* post(QUrl url, QByteArray payload);

    signals:

    private:
        GitHubHttpPrivate* d;
};

#endif // GITHUBHTTP_H
