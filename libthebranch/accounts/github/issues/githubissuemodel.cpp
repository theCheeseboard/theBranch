#include "githubissuemodel.h"

#include "../githubaccount.h"
#include "../pr/githubpullrequestapi.h"
#include "githubissue.h"
#include "githubissuesapi.h"

struct GitHubIssueModelPrivate {
        GitHubAccount* account;
        RemotePtr remote;
        bool isPr;
        QList<GitHubIssuePtr> issues;

        bool fetching = false;
        bool requireInitialFetch = true;
        QCoro::AsyncGenerator<GitHubIssuePtr> issueGenerator;
        QCoro::AsyncGenerator<GitHubIssuePtr>::iterator issueIterator = QCoro::AsyncGenerator<GitHubIssuePtr>::iterator(nullptr);
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

int GitHubIssueModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;

    return d->issues.length();
}

bool GitHubIssueModel::hasChildren(const QModelIndex& parent) const {
    return false;
}

bool GitHubIssueModel::canFetchMore(const QModelIndex& parent) const {
    if (d->requireInitialFetch) return true;
    if (d->fetching) return true;
    return d->issueIterator != d->issueGenerator.end();
}

void GitHubIssueModel::fetchMore(const QModelIndex& parent) {
    this->fetchNext();
}

QVariant GitHubIssueModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();

    auto issue = d->issues.at(index.row());
    switch (role) {
        case Qt::DisplayRole:
            return issue->title();
        case IssueRole:
            return QVariant::fromValue(issue);
    }

    return QVariant();
}

QCoro::Task<> GitHubIssueModel::startFetch() {
    beginRemoveRows(QModelIndex(), 0, this->rowCount());
    d->issues.clear();
    endRemoveRows();

    if (d->isPr) {
        d->issueGenerator = d->account->pr()->listPullRequests(d->remote);
    } else {
        d->issueGenerator = d->account->issues()->listIssues(d->remote);
    }

    d->issueIterator = co_await d->issueGenerator.begin();
}

QCoro::Task<> GitHubIssueModel::fetchNext() {
    if (d->fetching) co_return;

    d->fetching = true;
    if (d->requireInitialFetch) {
        d->requireInitialFetch = false;
        co_await startFetch();
    }

    for (auto i = 0; i < 100; i++) {
        if (d->issueIterator == d->issueGenerator.end()) {
            d->fetching = false;
            co_return; // Nothing else to add
        }

        emit beginInsertRows(QModelIndex(), this->rowCount(), this->rowCount() + 1);
        d->issues.append(*d->issueIterator);
        emit endInsertRows();

        co_await ++(d->issueIterator);
    }
    d->fetching = false;
}
