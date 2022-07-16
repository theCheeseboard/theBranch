#include "pushsnapin.h"
#include "ui_pushsnapin.h"

#include "objects/branch.h"
#include "objects/branchmodel.h"
#include "objects/libgit/lgrepository.h"
#include "objects/reference.h"
#include "objects/repository.h"
#include <tcontentsizer.h>

struct PushSnapInPrivate {
        RepositoryPtr repo;
        QModelIndex upstreamIndex;
};

PushSnapIn::PushSnapIn(RepositoryPtr repo, QWidget* parent) :
    SnapIn(parent),
    ui(new Ui::PushSnapIn) {
    ui->setupUi(this);
    d = new PushSnapInPrivate();
    d->repo = repo;

    ui->stackedWidget->setCurrentWidget(ui->pushOptionsPage);
    ui->titleLabel->setBackButtonShown(true);
    ui->titleLabel_2->setBackButtonShown(true);
    new tContentSizer(ui->pushOptionsWidget);
    new tContentSizer(ui->pushButton);
    new tContentSizer(ui->pushFailedWidget);
    new tContentSizer(ui->pullButton);
    ui->spinner->setFixedSize(SC_DPI_WT(QSize(32, 32), QSize, this));

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);

    // TODO: Create upstream branch
    auto* model = new BranchModel();
    model->setBranchFlags(THEBRANCH::RemoteBranches);
    model->setRepository(repo);
    ui->branchBox->setModel(model);

    auto branch = repo->head()->asBranch()->upstream();
    if (branch) {
        auto index = model->index(branch);
        if (index.isValid()) ui->branchBox->setCurrentIndex(index.row());
        d->upstreamIndex = index;
    }

    updateUpstreamBox();
}

PushSnapIn::~PushSnapIn() {
    delete ui;
    delete d;
}

void PushSnapIn::on_titleLabel_backButtonClicked() {
    emit done();
}

QCoro::Task<> PushSnapIn::on_pushButton_clicked() {
    auto branch = ui->branchBox->currentData(BranchModel::Branch).value<BranchPtr>();
    ui->stackedWidget->setCurrentWidget(ui->pushingPage);

    try {
        co_await d->repo->git_repository()->push(branch->remoteName(), branch->localBranchName(), ui->upstreamCheckbox->isChecked(), ui->tagsCheckbox->isChecked());
        emit done();
    } catch (const GitRepositoryOutOfDateException& ex) {
        ui->stackedWidget->setCurrentWidget(ui->pushFailedPage);
    }
}

void PushSnapIn::on_branchBox_currentIndexChanged(int index) {
    updateUpstreamBox();
}

void PushSnapIn::updateUpstreamBox() {
    if (ui->branchBox->currentIndex() == d->upstreamIndex.row()) {
        ui->upstreamCheckbox->setEnabled(false);
        ui->upstreamCheckbox->setChecked(true);
    } else {
        ui->upstreamCheckbox->setEnabled(true);
    }
}

void PushSnapIn::on_titleLabel_2_backButtonClicked() {
    emit done();
}
