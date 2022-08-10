#include "githubactionsworkflowbrowser.h"
#include "ui_githubactionsworkflowbrowser.h"

#include "githubactionsrunbrowser.h"
#include "githubworkflow.h"
#include "githubworkflowmodel.h"
#include <QTimer>

struct GitHubActionsWorkflowBrowserPrivate {
        GitHubWorkflowPtr workflow;
        GitHubWorkflowModel* workflowRuns;
};

GitHubActionsWorkflowBrowser::GitHubActionsWorkflowBrowser(GitHubWorkflowPtr workflow, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::GitHubActionsWorkflowBrowser) {
    ui->setupUi(this);
    d = new GitHubActionsWorkflowBrowserPrivate();
    d->workflow = workflow;

    ui->titleLabel->setText(tr("Runs for %1").arg(QLocale().quoteString(workflow->name())));
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);

    d->workflowRuns = new GitHubWorkflowModel(workflow);
    ui->listView->setModel(d->workflowRuns);
}

GitHubActionsWorkflowBrowser::~GitHubActionsWorkflowBrowser() {
    delete d;
    delete ui;
}

void GitHubActionsWorkflowBrowser::showWorkflowRun(GitHubWorkflowRunPtr workflowRun) {
    auto runsWidget = new GitHubActionsRunBrowser(workflowRun);
    connect(runsWidget, &GitHubActionsRunBrowser::goBack, this, [this, runsWidget] {
        ui->stackedWidget->setCurrentWidget(ui->runsPage);
        QTimer::singleShot(500, this, [this, runsWidget] {
            ui->stackedWidget->removeWidget(runsWidget);
            runsWidget->deleteLater();
        });
    });
    ui->stackedWidget->addWidget(runsWidget);
    ui->stackedWidget->setCurrentWidget(runsWidget, this->isVisible());
}

void GitHubActionsWorkflowBrowser::on_titleLabel_backButtonClicked() {
    emit goBack();
}

void GitHubActionsWorkflowBrowser::on_listView_clicked(const QModelIndex& index) {
    auto workflowRun = index.data(GitHubWorkflowModel::WorkflowRunRole).value<GitHubWorkflowRunPtr>();
    if (workflowRun) this->showWorkflowRun(workflowRun);
}
