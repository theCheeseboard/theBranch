#include "githubactionsrunbrowser.h"
#include "ui_githubactionsrunbrowser.h"

#include "githubworkflowrun.h"
#include "githubworkflowrunmodel.h"

struct GitHubActionsRunBrowserPrivate {
        GitHubWorkflowRunPtr workflowRun;
        GitHubWorkflowRunModel* jobs;
};

GitHubActionsRunBrowser::GitHubActionsRunBrowser(GitHubWorkflowRunPtr workflowRun, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::GitHubActionsRunBrowser) {
    ui->setupUi(this);

    d = new GitHubActionsRunBrowserPrivate();
    d->workflowRun = workflowRun;

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::Lift);

    ui->leftWidget->setFixedWidth(SC_DPI_W(300, this));
    ui->titleLabel->setText(workflowRun->headCommitMessage());

    d->jobs = new GitHubWorkflowRunModel(workflowRun);
    ui->listView->setModel(d->jobs);
}

GitHubActionsRunBrowser::~GitHubActionsRunBrowser() {
    delete d;
    delete ui;
}

void GitHubActionsRunBrowser::on_titleLabel_backButtonClicked() {
    emit goBack();
}
