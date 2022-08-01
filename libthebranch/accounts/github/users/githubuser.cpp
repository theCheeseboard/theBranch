#include "githubuser.h"

#include <QJsonObject>

struct GitHubUserPrivate {
        QString login;
};

GitHubUser::GitHubUser(GitHubAccount* account, RemotePtr remote) :
    GitHubItem{account, remote} {
    d = new GitHubUserPrivate();
}

GitHubUser::~GitHubUser() {
    delete d;
}

QString GitHubUser::login() {
    return d->login;
}

void GitHubUser::update(QJsonObject data) {
    d->login = data.value("login").toString();
    GitHubItem::update(data);
}

QCoro::Task<> GitHubUser::fetchLatest() {
    co_return;
}
