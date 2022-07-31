#include "githubitem.h"

#include <QJsonObject>

struct GitHubItemPrivate {
        QString nodeId;
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

void GitHubItem::update(QJsonObject data) {
    d->nodeId = data.value("node_id").toString();
    emit updated();
}
