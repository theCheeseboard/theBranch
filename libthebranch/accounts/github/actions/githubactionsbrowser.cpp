#include "githubactionsbrowser.h"
#include "ui_githubactionsbrowser.h"

#include "githubactionsmodel.h"
#include "githubactionsrunsbrowser.h"
#include <QTimer>

struct GitHubActionsBrowserPrivate {
        GitHubActionsModel* workflows;
};

GitHubActionsBrowser::GitHubActionsBrowser(GitHubAccount* account, RemotePtr remote, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::GitHubActionsBrowser) {
    ui->setupUi(this);
    d = new GitHubActionsBrowserPrivate();

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);

    d->workflows = new GitHubActionsModel(account, remote);
    ui->listView->setModel(d->workflows);
}

GitHubActionsBrowser::~GitHubActionsBrowser() {
    delete d;
    delete ui;
}

void GitHubActionsBrowser::showWorkflow(GitHubWorkflowPtr workflow) {
    auto runsWidget = new GitHubActionsRunsBrowser(workflow);
    connect(runsWidget, &GitHubActionsRunsBrowser::goBack, this, [this, runsWidget] {
        ui->stackedWidget->setCurrentWidget(ui->workflowsPage);
        QTimer::singleShot(500, this, [this, runsWidget] {
            ui->stackedWidget->removeWidget(runsWidget);
            runsWidget->deleteLater();
        });
    });
    ui->stackedWidget->addWidget(runsWidget);
    ui->stackedWidget->setCurrentWidget(runsWidget, this->isVisible());
}

void GitHubActionsBrowser::on_listView_clicked(const QModelIndex& index) {
    auto workflow = index.data(GitHubActionsModel::WorkflowRole).value<GitHubWorkflowPtr>();
    if (workflow) this->showWorkflow(workflow);
}
