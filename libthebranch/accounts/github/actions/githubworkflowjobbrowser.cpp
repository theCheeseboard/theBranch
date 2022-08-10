#include "githubworkflowjobbrowser.h"
#include "ui_githubworkflowjobbrowser.h"

#include "githubworkflowjob.h"
#include "githubworkflowjobmodel.h"

struct GitHubWorkflowJobBrowserPrivate {
        GitHubWorkflowJobPtr workflowJob;
        GitHubWorkflowJobModel* stepsModel;
};

GitHubWorkflowJobBrowser::GitHubWorkflowJobBrowser(GitHubWorkflowJobPtr workflowJob, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::GitHubWorkflowJobBrowser) {
    ui->setupUi(this);
    d = new GitHubWorkflowJobBrowserPrivate();
    d->workflowJob = workflowJob;

    ui->titleLabel->setText(workflowJob->name());

    d->stepsModel = new GitHubWorkflowJobModel(workflowJob);
    ui->listView->setModel(d->stepsModel);
}

GitHubWorkflowJobBrowser::~GitHubWorkflowJobBrowser() {
    delete d;
    delete ui;
}
