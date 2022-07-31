#include "githubitem.h"

#include <QJsonObject>

struct GitHubItemPrivate {
        GitHubAccount* account;
        RemotePtr remote;
        QString nodeId;
        QUrl htmlUrl;
};

GitHubItem::GitHubItem(GitHubAccount* account, RemotePtr remote) :
    QObject{nullptr} {
    d = new GitHubItemPrivate();
    d->account = account;
    d->remote = remote;
}

GitHubItem::~GitHubItem() {
    delete d;
}

GitHubAccount* GitHubItem::account() {
    return d->account;
}

RemotePtr GitHubItem::remote() {
    return d->remote;
}

QString GitHubItem::nodeId() {
    return d->nodeId;
}

QUrl GitHubItem::htmlUrl() {
    return d->htmlUrl;
}

void GitHubItem::update(QJsonObject data) {
    d->nodeId = data.value("node_id").toString();
    d->htmlUrl = data.value("html_url").toString();
    emit updated();
}

void GitHubItem::contextMenu(QMenu* menu) {
    // noop
}

QWidget* GitHubItem::widget() {
    return nullptr;
}
