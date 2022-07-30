#include "githubcreatepullrequestpopover.h"
#include "ui_githubcreatepullrequestpopover.h"

#include "accounts/github/githubaccount.h"
#include "accounts/github/githubhttp.h"
#include "githubpullrequestapi.h"
#include "objects/branch.h"
#include <tcontentsizer.h>

struct GitHubCreatePullRequestPopoverPrivate {
        GitHubAccount* account;
        BranchPtr from;
        BranchPtr to;
        RemotePtr toRemote;
};

GitHubCreatePullRequestPopover::GitHubCreatePullRequestPopover(GitHubAccount* account, BranchPtr from, BranchPtr to, RemotePtr toRemote, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::GitHubCreatePullRequestPopover) {
    ui->setupUi(this);

    d = new GitHubCreatePullRequestPopoverPrivate();
    d->account = account;
    d->from = from;
    d->to = to;
    d->toRemote = toRemote;

    ui->titleLabel->setBackButtonShown(true);
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
    new tContentSizer(ui->prOptionsWidget);
    new tContentSizer(ui->createPrButton);
    new tContentSizer(ui->prFailedFrame);
    ui->prFailedFrame->setVisible(false);

    ui->descriptionLabel->setText(tr("You are creating a pull request to merge %1 into %2").arg(QLocale().quoteString(from->name()), QLocale().quoteString(to->localBranchName())));
}

GitHubCreatePullRequestPopover::~GitHubCreatePullRequestPopover() {
    delete d;
    delete ui;
}

void GitHubCreatePullRequestPopover::on_titleLabel_backButtonClicked() {
    emit done();
}

QCoro::Task<> GitHubCreatePullRequestPopover::on_createPrButton_clicked() {
    ui->stackedWidget->setCurrentWidget(ui->loadingPage);
    try {
        co_await d->account->pr()->createPullRequest(d->toRemote, d->from, d->to, ui->titleEdit->text(), ui->commentEdit->toPlainText());
        emit done();
    } catch (GitHubException& ex) {
        QTimer::singleShot(500, this, [this, ex] {
            ui->prFailedFrame->setState(tStatusFrame::Error);
            ui->prFailedFrame->setTitle(tr("Could not create Pull Request"));
            ui->prFailedFrame->setText(ex.error());
            ui->prFailedFrame->setVisible(true);

            ui->stackedWidget->setCurrentWidget(ui->initialPage);
        });
    }
}
