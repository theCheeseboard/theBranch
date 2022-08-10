#include "githubactionsrunsbrowser.h"
#include "ui_githubactionsrunsbrowser.h"

GitHubActionsRunsBrowser::GitHubActionsRunsBrowser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GitHubActionsRunsBrowser)
{
    ui->setupUi(this);
}

GitHubActionsRunsBrowser::~GitHubActionsRunsBrowser()
{
    delete ui;
}
