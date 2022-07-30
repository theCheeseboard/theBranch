#include "githubhttp.h"

#include "githubaccount.h"
#include <QJsonArray>
#include <QNetworkAccessManager>
#include <QUrlQuery>

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

QUrl GitHubHttp::makeUrl(QString path, QUrlQuery query) {
    QUrl url;
    url.setScheme("https");
    url.setHost("api.github.com");
    url.setPath(path);
    url.setQuery(query);

    return url;
}

QNetworkReply* GitHubHttp::get(QString path) {
    return get(makeUrl(path));
}

QNetworkReply* GitHubHttp::get(QUrl url) {
    QNetworkRequest req(url);
    req.setRawHeader("Authorization", QByteArray("Basic ").append(QStringLiteral("%1:%2").arg(d->account->username(), d->account->token()).toUtf8().toBase64()));
    return d->mgr.get(req);
}

QNetworkReply* GitHubHttp::post(QString path, QByteArray payload) {
    return post(makeUrl(path), payload);
}

QNetworkReply* GitHubHttp::post(QUrl url, QByteArray payload) {
    QNetworkRequest req(url);
    req.setRawHeader("Authorization", QByteArray("Basic ").append(QStringLiteral("%1:%2").arg(d->account->username(), d->account->token()).toUtf8().toBase64()));
    return d->mgr.post(req, payload);
}

GitHubException::GitHubException(QString error) {
    this->_error = error;
}

GitHubException::GitHubException(QJsonObject response, QString defaultError) {
    if (!response.contains("errors")) {
        this->_error = defaultError;
        return;
    }
    auto errors = response.value("errors").toArray();
    if (errors.isEmpty()) {
        this->_error = defaultError;
        return;
    }

    auto error = errors.first().toObject();
    if (!error.contains("message")) {
        this->_error = defaultError;
        return;
    }

    this->_error = error.value("message").toString();
}

QString GitHubException::error() const {
    return _error;
}

void GitHubException::raise() const {
    throw *this;
}

GitHubException* GitHubException::clone() const {
    return new GitHubException(*this);
}
