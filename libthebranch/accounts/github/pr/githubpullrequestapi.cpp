#include "githubpullrequestapi.h"

#include <QCoroNetworkReply>
#include <QJsonArray>
#include <QJsonDocument>
#include <tlogger.h>

#include "../githubaccount.h"
#include "../githubhttp.h"
#include "objects/branch.h"
#include "objects/remote.h"

GitHubPullRequestApi::GitHubPullRequestApi(GitHubHttp* http) {
    this->http = http;
}

QCoro::Task<> GitHubPullRequestApi::createPullRequest(RemotePtr remote, BranchPtr from, BranchPtr to, QString title, QString body) {
    QJsonObject root;
    root.insert("head", from->name());
    root.insert("base", to->localBranchName());
    root.insert("title", title);
    root.insert("body", body);

    auto slug = remote->slugForAccount(http->account());
    QNetworkReply* reply = co_await this->http->post(QStringLiteral("/repos/%1/pulls").arg(slug), QJsonDocument(root).toJson());
    if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 201) {
        co_return;
    } else {
        QJsonObject obj = QJsonDocument::fromJson(reply->readAll()).object();
        throw GitHubException(obj, tr("Could not create Pull Request"));
    }
}

QCoro::AsyncGenerator<GitHubPullRequestApi::PullRequest> GitHubPullRequestApi::listPullRequests(RemotePtr remote, QString state) {
    auto slug = remote->slugForAccount(http->account());

    for (auto page = 1;; page++) {
        auto query = QUrlQuery({
            {"state",    state                },
            {"page",     QString::number(page)},
            {"per_page", "10"                 }
        });

        // ?? Looks like some compiler bug prevents us from co_await-ing this directly in AppleClang
        auto url = http->makeUrl(QStringLiteral("/repos/%1/pulls").arg(slug), query);
        QNetworkReply* reply = http->get(url);
        co_await qCoro(reply).waitForFinished();

        if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 200) {
            auto prs = QJsonDocument::fromJson(reply->readAll()).array();
            if (prs.isEmpty()) co_return;

            for (auto pr : prs) {
                co_yield PullRequest(pr.toObject());
            }
        } else {
            QJsonObject obj = QJsonDocument::fromJson(reply->readAll()).object();
            throw GitHubException(obj, tr("Could not create Pull Request"));
        }
    }
}

GitHubPullRequestApi::PullRequest::PullRequest(QJsonObject def) {
    this->id = def.value("id").toInteger();
    this->number = def.value("number").toInteger();
    this->title = def.value("title").toString();
    this->body = def.value("body").toString();
}
