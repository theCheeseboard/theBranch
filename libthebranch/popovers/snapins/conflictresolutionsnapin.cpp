#include "conflictresolutionsnapin.h"
#include "ui_conflictresolutionsnapin.h"

#include "objects/cherrypick.h"
#include "objects/gitoperation.h"
#include "objects/libgit/lgindex.h"
#include "objects/libgit/lgrepository.h"
#include "objects/merge.h"
#include "objects/rebase.h"
#include "objects/revert.h"
#include "objects/statusitemlistmodel.h"
#include "widgets/conflictresolution/textconflictresolution.h"
#include <tcontentsizer.h>
#include <tpopover.h>
#include <tscrim.h>

struct ConflictResolutionSnapInPrivate {
        GitOperationPtr gitOperation;
        StatusItemListFilterView* statusModel;

        QMap<QString, ConflictResolutionWidget*> conflictResolutionWidgets;
};

ConflictResolutionSnapIn::ConflictResolutionSnapIn(GitOperationPtr gitOperation, QWidget* parent) :
    SnapIn(parent),
    ui(new Ui::ConflictResolutionSnapIn) {
    ui->setupUi(this);

    d = new ConflictResolutionSnapInPrivate();
    d->gitOperation = gitOperation;

    ui->titleLabel->setBackButtonShown(true);
    ui->abortTitleLabel->setBackButtonShown(true);
    ui->leftWidget->setFixedWidth(300);
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
    ui->stackedWidget->setCurrentWidget(ui->conflictResolutionPage, false);
    ui->conflictResolutionStack->setCurrentAnimation(tStackedWidget::Lift);
    ui->doAbortButton->setProperty("type", "destructive");
    new tContentSizer(ui->abortConfirmWidget);

    if (gitOperation.objectCast<PullMerge>() || gitOperation.objectCast<PullRebase>()) {
        ui->abortExplainText->setText(tr("Aborting the conflict resolution at this point will return all files in the repository to the state they were in before you started pulling, and will also abort the pull operation. Any conflict resolution will be lost."));
        ui->continueConflictResolutionButton->setText(tr("Continue Pull"));
        ui->continueConflictResolutionButton->setIcon(QIcon::fromTheme("vcs-pull"));
        ui->doAbortButton->setText(tr("Abort Pull"));
        ui->completeButton->setText(tr("Pull"));
        ui->completeButton->setIcon(QIcon::fromTheme("vcs-pull"));
    } else if (gitOperation.objectCast<Merge>()) {
        ui->abortExplainText->setText(tr("Aborting the conflict resolution at this point will return all files in the repository to the state they were in before you started merging, and will also abort the merge operation. Any conflict resolution will be lost."));
        ui->continueConflictResolutionButton->setText(tr("Continue Merge"));
        ui->continueConflictResolutionButton->setIcon(QIcon::fromTheme("vcs-merge"));
        ui->doAbortButton->setText(tr("Abort Merge"));
        ui->completeButton->setText(tr("Merge"));
        ui->completeButton->setIcon(QIcon::fromTheme("vcs-merge"));
    } else if (gitOperation.objectCast<Rebase>()) {
        ui->abortExplainText->setText(tr("Aborting the conflict resolution at this point will return all files in the repository to the state they were in before you started rebasing, and will also abort the rebase operation. Any conflict resolution will be lost."));
        ui->continueConflictResolutionButton->setText(tr("Continue Rebase"));
        ui->continueConflictResolutionButton->setIcon(QIcon::fromTheme("vcs-rebase"));
        ui->doAbortButton->setText(tr("Abort Rebase"));
        ui->completeButton->setText(tr("Rebase"));
        ui->completeButton->setIcon(QIcon::fromTheme("vcs-rebase"));
    } else if (gitOperation.objectCast<CherryPick>()) {
        ui->abortExplainText->setText(tr("Aborting the conflict resolution at this point will return all files in the repository to the state they were in before you started cherry picking, and will also abort the cherry pick operation. Any conflict resolution will be lost."));
        ui->continueConflictResolutionButton->setText(tr("Continue Cherry Pick"));
        ui->continueConflictResolutionButton->setIcon(QIcon::fromTheme("vcs-cherry-pick"));
        ui->doAbortButton->setText(tr("Abort Cherry Pick"));
        ui->completeButton->setText(tr("Cherry Pick"));
        ui->completeButton->setIcon(QIcon::fromTheme("vcs-cherry-pick"));
    } else if (gitOperation.objectCast<Revert>()) {
        ui->abortExplainText->setText(tr("Aborting the conflict resolution at this point will return all files in the repository to the state they were in before you started the reversion, and will also abort the revert operation. Any conflict resolution will be lost."));
        ui->continueConflictResolutionButton->setText(tr("Continue Revert"));
        ui->continueConflictResolutionButton->setIcon(QIcon::fromTheme("vcs-revert"));
        ui->doAbortButton->setText(tr("Abort Revert"));
        ui->completeButton->setText(tr("Revert"));
        ui->completeButton->setIcon(QIcon::fromTheme("vcs-revert"));
    }

    StatusItemListModel* statusModel = new StatusItemListModel(this);
    statusModel->setUserCheckable(false);
    statusModel->setStatusItems(gitOperation->repository()->fileStatus());

    d->statusModel = new StatusItemListFilterView(this);
    d->statusModel->setSourceModel(statusModel);
    d->statusModel->setFlagFilters(Repository::StatusItem::Conflicting);
    ui->modifiedFilesEdit->setModel(d->statusModel);
    ui->modifiedFilesEdit->setItemDelegate(new StatusItemListDelegate(this));

    QDir repositoryDir(gitOperation->repository()->repositoryPath());

    for (auto i = 0; i < d->statusModel->rowCount(); i++) {
        auto index = d->statusModel->index(i, 0);
        auto path = index.data(StatusItemListModel::PathRole).toString();
        ConflictResolutionWidget* resolutionWidget;

        // TODO: Choose an appropriate widget
        resolutionWidget = new TextConflictResolution(repositoryDir.absoluteFilePath(path), this);

        ui->conflictResolutionStack->addWidget(resolutionWidget);

        connect(resolutionWidget, &ConflictResolutionWidget::conflictResolutionCompletedChanged, this, &ConflictResolutionSnapIn::updateConflictResolutionState);

        d->conflictResolutionWidgets.insert(path, resolutionWidget);
    }

    connect(ui->modifiedFilesEdit->selectionModel(), &QItemSelectionModel::currentRowChanged, this, [=](QModelIndex current) {
        if (current.isValid()) {
            ui->conflictResolutionStack->setCurrentWidget(d->conflictResolutionWidgets.value(current.data(StatusItemListModel::PathRole).toString()));
        }
    });

    this->updateConflictResolutionState();
}

ConflictResolutionSnapIn::~ConflictResolutionSnapIn() {
    delete d;
    delete ui;
}

void ConflictResolutionSnapIn::snapinShown() {
    tPopover::popoverForPopoverWidget(this)->setDismissable(false);
}

void ConflictResolutionSnapIn::on_titleLabel_backButtonClicked() {
    ui->stackedWidget->setCurrentWidget(ui->abortConflictResolutionPage);
}

void ConflictResolutionSnapIn::on_continueConflictResolutionButton_clicked() {
    ui->stackedWidget->setCurrentWidget(ui->conflictResolutionPage);
}

void ConflictResolutionSnapIn::on_doAbortButton_clicked() {
    d->gitOperation->abortOperation();
    emit done();
}

void ConflictResolutionSnapIn::updateConflictResolutionState() {
    bool resolutionCompleted = true;
    for (auto i = 0; i < d->statusModel->rowCount(); i++) {
        auto index = d->statusModel->index(i, 0);
        auto resolver = d->conflictResolutionWidgets.value(index.data(StatusItemListModel::PathRole).toString());
        auto conflictResolutionCompleted = resolver->isConflictResolutionCompleted();
        if (!conflictResolutionCompleted) resolutionCompleted = false;
        d->statusModel->setItemData(index, {
                                               {Qt::CheckStateRole, conflictResolutionCompleted ? Qt::Checked : Qt::Unchecked}
        });
    }

    ui->completeButton->setEnabled(resolutionCompleted);
}

void ConflictResolutionSnapIn::on_completeButton_clicked() {
    // Apply conflict resolution steps
    for (auto* resolver : d->conflictResolutionWidgets) {
        resolver->applyConflictResolution();
    }

    auto repo = d->gitOperation->repository();
    auto index = repo->git_repository()->index();

    for (auto item : repo->fileStatus()) {
        if (item.flags & Repository::StatusItem::Conflicting) {
            if (!index->addByPath(item.path)) {
                // TODO: Error handling
                return;
            }
        }
    }
    index->write();

    // Finalise the operation
    d->gitOperation->finaliseOperation();
    emit done();
}
