#include "githubhttp.h"

#include "githubaccount.h"
#include <QCoroNetworkReply>
#include <QJsonArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrlQuery>
#include <tapplication.h>

struct GitHubHttpPrivate {
        QNetworkAccessManager mgr;
        GitHubAccount* account;

        QMap<QUrl, QString> etags;
        QMap<QUrl, GitHubHttp::HttpResponse> cachedResponses;
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

QCoro::Task<GitHubHttp::HttpResponse> GitHubHttp::get(QString path) {
    return get(makeUrl(path));
}

QCoro::Task<GitHubHttp::HttpResponse> GitHubHttp::get(QUrl url) {
    QNetworkRequest req(url);
    if (d->etags.contains(url)) {
        //        req.setHeader(QNetworkRequest::IfNoneMatchHeader, QStringList({d->etags.value(url)}));
        req.setRawHeader("If-None-Match", d->etags.value(url).toUtf8());
    }
    req.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("%1/%2").arg(tApplication::applicationName(), tApplication::applicationVersion()));
    req.setRawHeader("Authorization", QByteArray("Basic ").append(QStringLiteral("%1:%2").arg(d->account->username(), d->account->token()).toUtf8().toBase64()));
    QNetworkReply* reply = co_await d->mgr.get(req);

    GitHubHttp::HttpResponse response(reply);
    if (response.statusCode == 304 && d->cachedResponses.contains(url)) {
        response = d->cachedResponses.value(url);
    } else {
        auto etag = reply->header(QNetworkRequest::ETagHeader).toString();
        d->etags.insert(url, etag);
        d->cachedResponses.insert(url, response);
    }

    co_return response;
}

QCoro::Task<GitHubHttp::HttpResponse> GitHubHttp::post(QString path, QByteArray payload) {
    return post(makeUrl(path), payload);
}

QCoro::Task<GitHubHttp::HttpResponse> GitHubHttp::post(QUrl url, QByteArray payload) {
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("%1/%2").arg(tApplication::applicationName(), tApplication::applicationVersion()));
    req.setRawHeader("Authorization", QByteArray("Basic ").append(QStringLiteral("%1:%2").arg(d->account->username(), d->account->token()).toUtf8().toBase64()));
    auto* reply = co_await d->mgr.post(req, payload);
    co_return reply;
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

GitHubHttp::HttpResponse::HttpResponse(QNetworkReply* reply) {
    this->body = reply->readAll();
    this->statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
}
