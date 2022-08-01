#include "githubissueevent.h"

#include "../githubaccount.h"
#include "../githubitemdatabase.h"
#include "../users/githubuser.h"
#include <QJsonObject>

struct GitHubIssueEventPrivate {
        QString event;
        QJsonObject eventProperties;
        GitHubUserPtr actor;
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

QVariant GitHubIssueEvent::eventProperty(QString property) {
    return d->eventProperties.value(property);
}

GitHubUserPtr GitHubIssueEvent::actor() {
    return d->actor;
}

void GitHubIssueEvent::update(QJsonObject data) {
    d->eventProperties = data;
    d->event = data.value("event").toString();

    d->actor = this->account()->itemDb()->update<GitHubUser>(this->account(), this->remote(), data.value("actor").toObject());

    GitHubItem::update(data);
}

QCoro::Task<> GitHubIssueEvent::fetchLatest() {
    co_return;
}
