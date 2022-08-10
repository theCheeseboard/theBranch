#include "githubaccount.h"

#include "actions/githubactionsapi.h"
#include "githubhttp.h"
#include "githubitemdatabase.h"
#include "issues/githubissuesapi.h"
#include "pr/githubpullrequestapi.h"
#include <QCoroNetworkReply>
#include <QNetworkAccessManager>
#include <QNetworkReply>

static QRegularExpression sshCloneUrl = QRegularExpression(QRegularExpression::anchoredPattern("git@github\\.com:(\\w+\\/\\w+)(?:\\.git)?"));
static QRegularExpression pathRegex = QRegularExpression(QRegularExpression::anchoredPattern("\\/(\\w+\\/\\w+)(?:\\.git)?"));

struct GitHubAccountPrivate {
        QString username;
        QString token;
        QString description;

        GitHubItemDatabase* itemDb;
        GitHubHttp* http;
        GitHubPullRequestApi* pr;
        GitHubIssuesApi* issues;
        GitHubActionsApi* actions;
};

GitHubAccount::GitHubAccount(QString username, QString token, QObject* parent) :
    AbstractAccount{parent} {
    this->init();
    d->username = username;
    d->token = token;
    d->description = "GitHub";
}

GitHubAccount::GitHubAccount(QJsonObject account, QObject* parent) :
    AbstractAccount{parent} {
    this->init();
    d->username = account.value("username").toString();
    d->token = account.value("token").toString();
    d->description = account.value("description").toString();
}

GitHubAccount::~GitHubAccount() {
    delete d->pr;
    delete d->issues;
    delete d->actions;
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

    QUrl cloneUrlUrl(cloneUrl);
    if (cloneUrlUrl.host() == "github.com") {
        auto pathMatch = pathRegex.match(cloneUrlUrl.path());
        if (pathMatch.hasMatch()) return pathMatch.captured(1);
    }
    return "";
}

GitHubItemDatabase* GitHubAccount::itemDb() {
    return d->itemDb;
}

GitHubPullRequestApi* GitHubAccount::pr() {
    return d->pr;
}

GitHubIssuesApi* GitHubAccount::issues() {
    return d->issues;
}

GitHubActionsApi* GitHubAccount::actions() {
    return d->actions;
}

QCoro::Task<bool> GitHubAccount::testConnection() {
    auto reply = co_await d->http->get("/user");
    co_return reply.statusCode == 200;
}

void GitHubAccount::init() {
    d = new GitHubAccountPrivate();
    d->http = new GitHubHttp(this);
    d->itemDb = new GitHubItemDatabase();
    d->issues = new GitHubIssuesApi(d->http);
    d->pr = new GitHubPullRequestApi(d->http);
    d->actions = new GitHubActionsApi(d->http);
}

QJsonObject GitHubAccount::save() {
    return {
        {"type",        "github"      },
        {"username",    d->username   },
        {"token",       d->token      },
        {"description", d->description},
    };
}
