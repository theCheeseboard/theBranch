#include "githubactionsbrowser.h"
#include "ui_githubactionsbrowser.h"

#include "githubactionsmodel.h"

struct GitHubActionsBrowserPrivate {
        GitHubActionsModel* workflows;
};

GitHubActionsBrowser::GitHubActionsBrowser(GitHubAccount* account, RemotePtr remote, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::GitHubActionsBrowser) {
    ui->setupUi(this);
    d = new GitHubActionsBrowserPrivate();

    d->workflows = new GitHubActionsModel(account, remote);
    ui->listView->setModel(d->workflows);
}

GitHubActionsBrowser::~GitHubActionsBrowser() {
    delete d;
    delete ui;
}
