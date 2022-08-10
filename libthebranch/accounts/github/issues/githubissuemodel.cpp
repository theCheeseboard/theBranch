#include "githubissuemodel.h"

#include "../githubaccount.h"
#include "../pr/githubpullrequestapi.h"
#include "githubissue.h"
#include "githubissuesapi.h"

struct GitHubIssueModelPrivate {
        GitHubAccount* account;
        RemotePtr remote;
        bool isPr;
};

GitHubIssueModel::GitHubIssueModel(GitHubAccount* account, RemotePtr remote, bool isPr, QObject* parent) :
    QAbstractListModel(parent) {
    d = new GitHubIssueModelPrivate();
    d->account = account;
    d->remote = remote;
    d->isPr = isPr;
}

GitHubIssueModel::~GitHubIssueModel() {
    delete d;
}

QVariant GitHubIssueModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();

    auto issue = item(index);
    switch (role) {
        case Qt::DisplayRole:
            return issue->title();
        case IssueRole:
            return QVariant::fromValue(issue);
    }

    return QVariant();
}

QCoro::AsyncGenerator<GitHubIssuePtr> GitHubIssueModel::createGenerator() {
    if (d->isPr) {
        return d->account->pr()->listPullRequests(d->remote);
    } else {
        return d->account->issues()->listIssues(d->remote);
    }
}
