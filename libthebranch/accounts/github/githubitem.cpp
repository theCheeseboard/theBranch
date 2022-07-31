#include "githubitem.h"

#include <QJsonObject>

struct GitHubItemPrivate {
        QString nodeId;
        QUrl htmlUrl;
};

GitHubItem::GitHubItem() :
    QObject{nullptr} {
    d = new GitHubItemPrivate();
}

GitHubItem::~GitHubItem() {
    delete d;
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
