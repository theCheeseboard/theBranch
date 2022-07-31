#include "githubissuesapi.h"

#include "../githubaccount.h"
#include "../githubhttp.h"
#include "../githubitemdatabase.h"
#include "../pr/githubpullrequest.h"
#include "githubissue.h"
#include "objects/remote.h"
#include <QCoroNetworkReply>
#include <QJsonArray>
#include <QJsonDocument>

GitHubIssuesApi::GitHubIssuesApi(GitHubHttp* http) {
    this->http = http;
}

QCoro::AsyncGenerator<GitHubIssuePtr> GitHubIssuesApi::listIssues(RemotePtr remote, QString state) {
    auto slug = remote->slugForAccount(http->account());

    for (auto page = 1;; page++) {
        auto query = QUrlQuery({
            {"state",    state                },
            {"page",     QString::number(page)},
            {"per_page", "10"                 }
        });

        // ?? Looks like some compiler bug prevents us from co_await-ing this directly in AppleClang
        auto url = http->makeUrl(QStringLiteral("/repos/%1/issues").arg(slug), query);
        QNetworkReply* reply = http->get(url);
        co_await qCoro(reply).waitForFinished();

        if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 200) {
            auto issues = QJsonDocument::fromJson(reply->readAll()).array();
            if (issues.isEmpty()) co_return;

            for (auto issue : issues) {
                auto issueObj = issue.toObject();
                if (issueObj.contains("pull_request")) continue; // Ignore PRs
                auto ghIssue = http->account()->itemDb()->update<GitHubIssue>(http->account(), remote, issueObj);
                co_yield ghIssue;
            }
        } else {
            QJsonObject obj = QJsonDocument::fromJson(reply->readAll()).object();
            throw GitHubException(obj, tr("Could not get issues"));
        }
    }
}

QCoro::Task<GitHubIssuePtr> GitHubIssuesApi::issue(RemotePtr remote, qint64 issueNumber) {
    auto slug = remote->slugForAccount(http->account());
    QNetworkReply* reply = co_await http->get(QStringLiteral("/repos/%1/issues/%2").arg(slug).arg(issueNumber));

    if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 200) {
        auto issue = QJsonDocument::fromJson(reply->readAll()).object();

        if (issue.contains("pull_request")) {
            co_return http->account()->itemDb()->update<GitHubPullRequest>(http->account(), remote, issue);
        } else {
            co_return http->account()->itemDb()->update<GitHubIssue>(http->account(), remote, issue);
        }
    } else {
        QJsonObject obj = QJsonDocument::fromJson(reply->readAll()).object();
        throw GitHubException(obj, tr("Could not get issues"));
    }
}
