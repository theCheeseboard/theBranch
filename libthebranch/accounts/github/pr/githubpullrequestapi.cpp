#include "githubpullrequestapi.h"

#include <QCoroNetworkReply>
#include <QJsonArray>
#include <QJsonDocument>
#include <tlogger.h>

#include "../githubaccount.h"
#include "../githubhttp.h"
#include "../githubitemdatabase.h"
#include "githubpullrequest.h"
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
    auto reply = co_await this->http->post(QStringLiteral("/repos/%1/pulls").arg(slug), QJsonDocument(root).toJson());
    if (reply.statusCode == 201) {
        co_return;
    } else {
        QJsonObject obj = QJsonDocument::fromJson(reply.body).object();
        throw GitHubException(obj, tr("Could not create Pull Request"));
    }
}

QCoro::AsyncGenerator<GitHubIssuePtr> GitHubPullRequestApi::listPullRequests(RemotePtr remote, QString state) {
    auto slug = remote->slugForAccount(http->account());

    for (auto page = 1;; page++) {
        auto query = QUrlQuery({
            {"state",    state                },
            {"page",     QString::number(page)},
            {"per_page", "10"                 }
        });

        auto reply = co_await http->get(http->makeUrl(QStringLiteral("/repos/%1/pulls").arg(slug), query));

        if (reply.statusCode == 200) {
            auto prs = QJsonDocument::fromJson(reply.body).array();
            if (prs.isEmpty()) co_return;

            for (auto pr : prs) {
                auto ghIssue = http->account()->itemDb()->update<GitHubPullRequest>(http->account(), remote, pr.toObject());
                co_yield ghIssue;
            }
        } else {
            QJsonObject obj = QJsonDocument::fromJson(reply.body).object();
            throw GitHubException(obj, tr("Could not get Pull Requests"));
        }
    }
}
