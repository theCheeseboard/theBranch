#include "pushsnapin.h"
#include "newremotesnapin.h"
#include "ui_pushsnapin.h"

#include "../snapinpopover.h"
#include "objects/branch.h"
#include "objects/branchmodel.h"
#include "objects/commit.h"
#include "objects/libgit/lgrepository.h"
#include "objects/reference.h"
#include "objects/repository.h"
#include "pullsnapin.h"
#include <tcontentsizer.h>

struct PushSnapInPrivate {
        RepositoryPtr repo;
        QModelIndex upstreamIndex;
        bool awaitingPull = false;
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
    ui->titleLabel_3->setBackButtonShown(true);
    new tContentSizer(ui->pushOptionsWidget);
    new tContentSizer(ui->pushButton);
    new tContentSizer(ui->pushFailedWidget);
    new tContentSizer(ui->pullButton);
    new tContentSizer(ui->noUpstreamFrame);
    new tContentSizer(ui->pushFailedFrame);
    new tContentSizer(ui->forcePushWidget);
    new tContentSizer(ui->doForcePushButton);
    new tContentSizer(ui->pushFailedForcePushButton);
    new tContentSizer(ui->forcePushButton);
    ui->spinner->setFixedSize(SC_DPI_WT(QSize(32, 32), QSize, this));

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
    ui->forcePushButton->setProperty("type", "destructive");
    ui->doForcePushButton->setProperty("type", "destructive");
    ui->pushFailedForcePushButton->setProperty("type", "destructive");

    // TODO: Create upstream branch
    auto* model = new BranchModel();
    model->setBranchFlags(THEBRANCH::RemoteBranches);
    model->setRepository(repo);
    model->setFakePushBranchName(repo->head()->asBranch()->name());
    ui->branchBox->setModel(model);

    auto branch = repo->head()->asBranch()->upstream();
    if (branch) {
        auto name = branch->name();
        auto index = model->index(branch);
        if (index.isValid()) ui->branchBox->setCurrentIndex(index.row());
        d->upstreamIndex = index;
    }

    ui->noUpstreamFrame->setState(tStatusFrame::Error);
    ui->noUpstreamFrame->setTitle(tr("No Remotes Configured"));
    ui->noUpstreamFrame->setText(tr("To push from this repository, you need to add a remote."));
    connect(model, &BranchModel::dataChanged, this, [this, model] {
        ui->noUpstreamFrame->setVisible(model->rowCount() == 0);
    });
    ui->noUpstreamFrame->setVisible(model->rowCount() == 0);

    auto addRemoteButton = ui->noUpstreamFrame->addButton();
    addRemoteButton->setText(tr("Add Remote"));
    addRemoteButton->setIcon(QIcon::fromTheme("list-add"));
    connect(addRemoteButton, &QPushButton::clicked, this, [this] {
        this->parentPopover()->pushSnapIn(new NewRemoteSnapIn(d->repo));
    });

    ui->pushFailedFrame->setState(tStatusFrame::Error);
    ui->pushFailedFrame->setTitle(tr("Failed to push"));
    ui->pushFailedFrame->setText(tr("Unable to push the repository to the remote"));
    ui->pushFailedFrame->setVisible(false);

    updateUpstreamBox();
    updatePushButton();
}

PushSnapIn::~PushSnapIn() {
    delete ui;
    delete d;
}

void PushSnapIn::on_titleLabel_backButtonClicked() {
    emit done();
}

QCoro::Task<> PushSnapIn::on_pushButton_clicked() {
    co_await doPush(false);
}

void PushSnapIn::on_branchBox_currentIndexChanged(int index) {
    updateUpstreamBox();
    updatePushButton();
}

void PushSnapIn::updateUpstreamBox() {
    if (ui->branchBox->currentIndex() == d->upstreamIndex.row()) {
        ui->upstreamCheckbox->setEnabled(false);
        ui->upstreamCheckbox->setChecked(true);
    } else {
        ui->upstreamCheckbox->setEnabled(true);
    }
}

void PushSnapIn::updatePushButton() {
    ui->pushButton->setEnabled(ui->branchBox->currentIndex() != -1);

    auto branch = ui->branchBox->currentData(BranchModel::Branch).value<BranchPtr>();
    ui->forcePushButton->setEnabled(!branch.isNull());
    if (!branch) {
        ui->pushButton->setText(tr("Publish Branch"));
        return;
    }

    auto lastCommitOnBranch = branch->lastCommit();
    auto headCommit = d->repo->head()->asCommit();
    ui->pushButton->setText(tr("Push %n commits", nullptr, headCommit->missingCommits(lastCommitOnBranch)));
}

void PushSnapIn::prepareForcePush() {
    auto branch = ui->branchBox->currentData(BranchModel::Branch).value<BranchPtr>();
    ui->doForcePushButton->setText(tr("Force Push %1 to %2").arg(QLocale().quoteString(d->repo->head()->asBranch()->name())).arg(QLocale().quoteString(branch->name())));
    ui->stackedWidget->setCurrentWidget(ui->forcePushPage);
}

QCoro::Task<> PushSnapIn::doPush(bool force) {
    QString remoteName;
    QString localBranchName;
    auto branch = ui->branchBox->currentData(BranchModel::Branch).value<BranchPtr>();
    if (!branch) {
        remoteName = ui->branchBox->currentData(BranchModel::FakeRemoteName).toString();
        localBranchName = d->repo->head()->asBranch()->name();
    } else {
        remoteName = branch->remoteName();
        localBranchName = branch->localBranchName();
    }
    ui->stackedWidget->setCurrentWidget(ui->pushingPage);

    try {
        co_await d->repo->git_repository()->push(remoteName, localBranchName, force, ui->upstreamCheckbox->isChecked(), ui->tagsCheckbox->isChecked(), this->parentPopover()->getInformationRequiredCallback());
        emit done();
    } catch (const GitRepositoryOutOfDateException& ex) {
        ui->stackedWidget->setCurrentWidget(ui->pushFailedPage);
    } catch (const GitException& ex) {
        QTimer::singleShot(500, this, [this, ex, branch] {
            ui->pushFailedFrame->setVisible(true);
            ui->pushFailedForcePushButton->setVisible(!branch.isNull());
            ui->stackedWidget->setCurrentWidget(ui->pushOptionsPage);
            ui->pushFailedFrame->setText(ex.description());
        });
    } catch (const QException& ex) {
        QTimer::singleShot(500, this, [this, branch] {
            ui->pushFailedFrame->setVisible(true);
            ui->pushFailedForcePushButton->setVisible(!branch.isNull());
            ui->stackedWidget->setCurrentWidget(ui->pushOptionsPage);
            ui->pushFailedFrame->setText(tr("Unable to push the repository to the remote"));
        });
    }
}

void PushSnapIn::on_titleLabel_2_backButtonClicked() {
    emit done();
}

void PushSnapIn::on_pullButton_clicked() {
    this->parentPopover()->pushSnapIn(new PullSnapIn(d->repo));
    d->awaitingPull = true;
}

void PushSnapIn::snapinShown() {
    if (d->awaitingPull) {
        ui->stackedWidget->setCurrentWidget(ui->pushOptionsPage, false);
        d->awaitingPull = false;
    }
}

void PushSnapIn::on_titleLabel_3_backButtonClicked() {
    ui->stackedWidget->setCurrentWidget(ui->pushOptionsPage);
}

void PushSnapIn::on_forcePushButton_clicked() {
    prepareForcePush();
}

void PushSnapIn::on_pushFailedForcePushButton_clicked() {
    prepareForcePush();
}

void PushSnapIn::on_doForcePushButton_clicked() {
    doPush(true);
}
