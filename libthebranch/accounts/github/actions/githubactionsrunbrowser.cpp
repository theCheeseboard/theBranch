#include "githubactionsrunbrowser.h"
#include "ui_githubactionsrunbrowser.h"

#include "githubworkflowjobbrowser.h"
#include "githubworkflowrun.h"
#include "githubworkflowrunmodel.h"
#include <QTimer>

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

    connect(ui->listView->selectionModel(), &QItemSelectionModel::selectionChanged, this, [this] {
        auto currentWidget = ui->stackedWidget->widget(ui->stackedWidget->currentIndex());
        if (auto currentJobBrowser = qobject_cast<GitHubWorkflowJobBrowser*>(currentWidget)) {
            QTimer::singleShot(500, this, [this, currentJobBrowser] {
                ui->stackedWidget->removeWidget(currentJobBrowser);
                currentJobBrowser->deleteLater();
            });
        }

        auto selection = ui->listView->selectionModel()->selectedIndexes();
        if (selection.empty()) {
            ui->stackedWidget->setCurrentWidget(ui->summaryPage);
        } else {
            auto index = selection.first();
            auto workflowJob = index.data(GitHubWorkflowRunModel::WorkflowRunRole).value<GitHubWorkflowJobPtr>();

            auto jobWidget = new GitHubWorkflowJobBrowser(workflowJob);
            ui->stackedWidget->addWidget(jobWidget);
            ui->stackedWidget->setCurrentWidget(jobWidget, this->isVisible());
        }
    });
}

GitHubActionsRunBrowser::~GitHubActionsRunBrowser() {
    delete d;
    delete ui;
}

void GitHubActionsRunBrowser::on_titleLabel_backButtonClicked() {
    emit goBack();
}
