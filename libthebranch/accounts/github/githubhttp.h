#ifndef GITHUBHTTP_H
#define GITHUBHTTP_H

#include <QCoroTask>
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

        struct HttpResponse {
                HttpResponse() = default;
                HttpResponse(QNetworkReply* reply);
                HttpResponse(const HttpResponse& other) = default;

                QByteArray body;
                int statusCode;
        };

        GitHubAccount* account();

        QUrl makeUrl(QString path, QUrlQuery query = {});

        QCoro::Task<HttpResponse> get(QString path);
        QCoro::Task<HttpResponse> get(QUrl url);

        QCoro::Task<HttpResponse> post(QString path, QByteArray payload);
        QCoro::Task<HttpResponse> post(QUrl url, QByteArray payload);

        QCoro::Task<HttpResponse> patch(QString path, QByteArray payload);
        QCoro::Task<HttpResponse> patch(QUrl url, QByteArray payload);

    signals:

    private:
        GitHubHttpPrivate* d;
};

#endif // GITHUBHTTP_H
