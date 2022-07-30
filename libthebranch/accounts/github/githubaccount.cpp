#include "githubaccount.h"

#include "githubhttp.h"
#include "pr/githubpullrequestapi.h"
#include <QCoroNetworkReply>
#include <QNetworkAccessManager>
#include <QNetworkReply>

static QRegularExpression sshCloneUrl = QRegularExpression(QRegularExpression::anchoredPattern("git@github\\.com:(\\w+\\/\\w+)(?:\\.git)?"));

struct GitHubAccountPrivate {
        QString username;
        QString token;
        QString description;

        GitHubHttp* http;
};

GitHubAccount::GitHubAccount(QString username, QString token, QObject* parent) {
    d = new GitHubAccountPrivate();
    d->username = username;
    d->token = token;
    d->description = "GitHub";
    d->http = new GitHubHttp(this);
}

GitHubAccount::GitHubAccount(QJsonObject account, QObject* parent) :
    AbstractAccount{parent} {
    d = new GitHubAccountPrivate();
    d->username = account.value("username").toString();
    d->token = account.value("token").toString();
    d->description = account.value("description").toString();
    d->http = new GitHubHttp(this);
}

GitHubAccount::~GitHubAccount() {
    d->http->deleteLater();
    delete d;
}

QString GitHubAccount::username() {
    return d->username;
}

QString GitHubAccount::token() {
    return d->token;
}

QString GitHubAccount::description() {
    return d->description;
}

QString GitHubAccount::slugForCloneUrl(QString cloneUrl) {
    auto sshMatch = sshCloneUrl.match(cloneUrl);
    if (sshMatch.hasMatch()) {
        return sshMatch.captured(1);
    }
    return "";
}

GitHubPullRequestApi GitHubAccount::pr() {
    return GitHubPullRequestApi(d->http);
}

QCoro::Task<bool> GitHubAccount::testConnection() {
    QNetworkReply* reply = co_await d->http->get("/user");
    co_return reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 200;
}

QJsonObject GitHubAccount::save() {
    return {
        {"type",        "github"      },
        {"username",    d->username   },
        {"token",       d->token      },
        {"description", d->description},
    };
}
