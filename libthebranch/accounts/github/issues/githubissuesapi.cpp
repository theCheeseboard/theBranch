#include "githubissuesapi.h"

#include "../githubaccount.h"
#include "../githubhttp.h"
#include "../githubitemdatabase.h"
#include "../pr/githubpullrequest.h"
#include "githubissue.h"
#include "githubissuecommentevent.h"
#include "githubissueevent.h"
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
            {"per_page", "100"                }
        });

        auto reply = co_await http->get(http->makeUrl(QStringLiteral("/repos/%1/issues").arg(slug), query));

        if (reply.statusCode == 200) {
            auto issues = QJsonDocument::fromJson(reply.body).array();
            if (issues.isEmpty()) co_return;

            for (auto issue : issues) {
                auto issueObj = issue.toObject();
                if (issueObj.contains("pull_request")) continue; // Ignore PRs
                auto ghIssue = http->account()->itemDb()->update<GitHubIssue>(http->account(), remote, issueObj);
                co_yield ghIssue;
            }
        } else {
            QJsonObject obj = QJsonDocument::fromJson(reply.body).object();
            throw GitHubException(obj, tr("Could not get issues"));
        }
    }
}

QCoro::Task<GitHubIssuePtr> GitHubIssuesApi::issue(RemotePtr remote, qint64 issueNumber) {
    auto slug = remote->slugForAccount(http->account());
    auto reply = co_await http->get(QStringLiteral("/repos/%1/issues/%2").arg(slug).arg(issueNumber));

    if (reply.statusCode == 200) {
        auto issue = QJsonDocument::fromJson(reply.body).object();

        if (issue.contains("pull_request")) {
            co_return http->account()->itemDb()->update<GitHubPullRequest>(http->account(), remote, issue);
        } else {
            co_return http->account()->itemDb()->update<GitHubIssue>(http->account(), remote, issue);
        }
    } else {
        QJsonObject obj = QJsonDocument::fromJson(reply.body).object();
        throw GitHubException(obj, tr("Could not get issues"));
    }
}

QCoro::AsyncGenerator<GitHubIssueEventPtr> GitHubIssuesApi::listIssueEvents(RemotePtr remote, qint64 issueNumber) {
    auto slug = remote->slugForAccount(http->account());

    for (auto page = 1;; page++) {
        auto query = QUrlQuery({
            {"page",     QString::number(page)},
            {"per_page", "100"                }
        });

        auto reply = co_await http->get(http->makeUrl(QStringLiteral("/repos/%1/issues/%2/timeline").arg(slug).arg(issueNumber), query));

        if (reply.statusCode == 200) {
            auto events = QJsonDocument::fromJson(reply.body).array();
            if (events.isEmpty()) co_return;

            for (auto event : events) {
                auto eventObj = event.toObject();

                if (QStringList({"commented", "reviewed"}).contains(eventObj.value("event").toString())) {
                    co_yield http->account()->itemDb()->update<GitHubIssueCommentEvent>(http->account(), remote, eventObj);
                } else {
                    co_yield http->account()->itemDb()->update<GitHubIssueEvent>(http->account(), remote, eventObj);
                }
            }
        } else {
            QJsonObject obj = QJsonDocument::fromJson(reply.body).object();
            throw GitHubException(obj, tr("Could not get issue timeline"));
        }
    }
}

QCoro::Task<> GitHubIssuesApi::updateIssue(RemotePtr remote, qint64 issueNumber, QJsonObject fieldsToUpdate) {
    auto slug = remote->slugForAccount(http->account());
    auto reply = co_await http->patch(QStringLiteral("/repos/%1/issues/%2").arg(slug).arg(issueNumber), QJsonDocument(fieldsToUpdate).toJson());

    if (reply.statusCode == 200) {
        auto issue = QJsonDocument::fromJson(reply.body).object();

        if (issue.contains("pull_request")) {
            http->account()->itemDb()->update<GitHubPullRequest>(http->account(), remote, issue);
        } else {
            http->account()->itemDb()->update<GitHubIssue>(http->account(), remote, issue);
        }
    } else {
        QJsonObject obj = QJsonDocument::fromJson(reply.body).object();
        throw GitHubException(obj, tr("Could not update issue"));
    }
}

QCoro::Task<> GitHubIssuesApi::postComment(RemotePtr remote, qint64 issueNumber, QString commentBody) {
    QJsonObject payload;
    payload.insert("body", commentBody);

    auto slug = remote->slugForAccount(http->account());
    auto reply = co_await http->post(QStringLiteral("/repos/%1/issues/%2/comments").arg(slug).arg(issueNumber), QJsonDocument(payload).toJson());

    if (reply.statusCode == 201) {
        co_return;
    } else {
        QJsonObject obj = QJsonDocument::fromJson(reply.body).object();
        throw GitHubException(obj, tr("Could not post comment"));
    }
}
