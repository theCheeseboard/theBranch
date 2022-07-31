#include "githubissuebrowser.h"
#include "ui_githubissuebrowser.h"

struct GitHubIssueBrowserPrivate {
        GitHubAccount* account;
        RemotePtr remote;
        bool isPr;
};

GitHubIssueBrowser::GitHubIssueBrowser(GitHubAccount* account, RemotePtr remote, bool isPr, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::GitHubIssueBrowser) {
    ui->setupUi(this);

    d = new GitHubIssueBrowserPrivate();
    d->account = account;
    d->remote = remote;
    d->isPr = isPr;

    ui->titleLabel->setText(isPr ? tr("Pull Requests") : tr("Issues"));
    ui->titleLabel->setBackButtonIsMenu(true);
    ui->titleLabel->setBackButtonShown(true);
}

GitHubIssueBrowser::~GitHubIssueBrowser() {
    delete d;
    delete ui;
}
