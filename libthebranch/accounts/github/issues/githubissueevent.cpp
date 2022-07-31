#include "githubissueevent.h"

#include <QJsonObject>

struct GitHubIssueEventPrivate {
        QString event;
};

GitHubIssueEvent::GitHubIssueEvent(GitHubAccount* account, RemotePtr remote) :
    GitHubItem{account, remote} {
    d = new GitHubIssueEventPrivate();
}

GitHubIssueEvent::~GitHubIssueEvent() {
    delete d;
}

QString GitHubIssueEvent::event() {
    return d->event;
}

void GitHubIssueEvent::update(QJsonObject data) {
    d->event = data.value("event").toString();
    GitHubItem::update(data);
}

QCoro::Task<> GitHubIssueEvent::fetchLatest() {
    co_return;
}
