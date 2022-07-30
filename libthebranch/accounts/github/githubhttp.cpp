#include "githubhttp.h"

#include "githubaccount.h"
#include <QNetworkAccessManager>

struct GitHubHttpPrivate {
        QNetworkAccessManager mgr;
        GitHubAccount* account;
};

GitHubHttp::GitHubHttp(GitHubAccount* account, QObject* parent) :
    QObject{parent} {
    d = new GitHubHttpPrivate;
    d->account = account;
}

GitHubHttp::~GitHubHttp() {
    delete d;
}

GitHubAccount* GitHubHttp::account() {
    return d->account;
}

QNetworkReply* GitHubHttp::get(QString path) {
    QUrl url;
    url.setScheme("https");
    url.setHost("api.github.com");
    url.setPath(path);

    QNetworkRequest req(url);
    req.setRawHeader("Authorization", QByteArray("Basic ").append(QStringLiteral("%1:%2").arg(d->account->username(), d->account->token()).toUtf8().toBase64()));
    return d->mgr.get(req);
}

QNetworkReply* GitHubHttp::post(QString path, QByteArray payload) {
    QUrl url;
    url.setScheme("https");
    url.setHost("api.github.com");
    url.setPath(path);

    QNetworkRequest req(url);
    req.setRawHeader("Authorization", QByteArray("Basic ").append(QStringLiteral("%1:%2").arg(d->account->username(), d->account->token()).toUtf8().toBase64()));
    return d->mgr.post(req, payload);
}
