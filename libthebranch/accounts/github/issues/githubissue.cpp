#include "githubissue.h"

#include <QJsonObject>

struct GitHubIssuePrivate {
        qint64 number;
        QString title;
        QString body;
};

GitHubIssue::GitHubIssue() :
    GitHubItem{} {
    d = new GitHubIssuePrivate();
}

GitHubIssue::~GitHubIssue() {
    delete d;
}

qint64 GitHubIssue::number() {
    return d->number;
}

QString GitHubIssue::title() {
    return d->title;
}

QString GitHubIssue::body() {
    return d->body;
}

void GitHubIssue::update(QJsonObject data) {
    d->number = data.value("number").toInteger();
    d->title = data.value("title").toString();
    d->body = data.value("body").toString();
    GitHubItem::update(data);
}
