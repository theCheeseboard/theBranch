#include "addgithubaccountpopover.h"
#include "ui_addgithubaccountpopover.h"

#include "../accountsmanager.h"
#include "githubaccount.h"
#include <QDesktopServices>

AddGithubAccountPopover::AddGithubAccountPopover(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::AddGithubAccountPopover) {
    ui->setupUi(this);

    ui->titleLabel->setBackButtonShown(true);
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
}

AddGithubAccountPopover::~AddGithubAccountPopover() {
    delete ui;
}

void AddGithubAccountPopover::on_titleLabel_backButtonClicked() {
    emit done();
}

QCoro::Task<> AddGithubAccountPopover::on_addButton_clicked() {
    ui->stackedWidget->setCurrentWidget(ui->loadingPage);
    auto* account = new GitHubAccount(ui->usernameBox->text(), ui->patBox->text());
    auto success = co_await account->testConnection();
    if (success) {
        AccountsManager::instance()->addAccount(account);
        emit done();
    } else {
        account->deleteLater();

        QTimer::singleShot(500, this, [this] {
            ui->stackedWidget->setCurrentWidget(ui->loginPage);
        });
    }
}

void AddGithubAccountPopover::on_createPatButton_clicked() {
    QDesktopServices::openUrl(QUrl("https://github.com/settings/tokens"));
}
