#include "githubactionsrunsbrowser.h"
#include "ui_githubactionsrunsbrowser.h"

#include "githubworkflow.h"
#include "githubworkflowmodel.h"

struct GitHubActionsRunsBrowserPrivate {
        GitHubWorkflowPtr workflow;
        GitHubWorkflowModel* workflowRuns;
};

GitHubActionsRunsBrowser::GitHubActionsRunsBrowser(GitHubWorkflowPtr workflow, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::GitHubActionsRunsBrowser) {
    ui->setupUi(this);
    d = new GitHubActionsRunsBrowserPrivate();
    d->workflow = workflow;

    ui->titleLabel->setText(tr("Runs for %1").arg(QLocale().quoteString(workflow->name())));

    d->workflowRuns = new GitHubWorkflowModel(workflow);
    ui->listView->setModel(d->workflowRuns);
}

GitHubActionsRunsBrowser::~GitHubActionsRunsBrowser() {
    delete d;
    delete ui;
}

void GitHubActionsRunsBrowser::on_titleLabel_backButtonClicked() {
    emit goBack();
}
