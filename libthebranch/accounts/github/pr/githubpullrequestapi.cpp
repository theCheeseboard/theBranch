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

QCoro::Task<QString> GitHubPullRequestApi::createPullRequest(RemotePtr remote, BranchPtr from, BranchPtr to, QString title, QString body) {
    QJsonObject root;
    root.insert("head", from->name());
    root.insert("base", to->localBranchName());
    root.insert("title", title);
    root.insert("body", body);

    auto slug = remote->slugForAccount(http->account());
    QNetworkReply* reply = co_await this->http->post(QStringLiteral("/repos/%1/pulls").arg(slug), QJsonDocument(root).toJson());
    if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 201) {
        co_return "";
    } else {
        QJsonObject obj = QJsonDocument::fromJson(reply->readAll()).object();
        if (!obj.contains("errors")) co_return tr("Could not create Pull Request");
        auto errors = obj.value("errors").toArray();
        if (errors.isEmpty()) co_return tr("Could not create Pull Request");
        auto error = errors.first().toObject();
        if (!error.contains("message")) co_return tr("Could not create Pull Request");
        co_return error.value("message").toString();
    }
}
