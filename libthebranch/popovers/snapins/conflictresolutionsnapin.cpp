#include "conflictresolutionsnapin.h"
#include "ui_conflictresolutionsnapin.h"

#include "objects/gitoperation.h"
#include "objects/merge.h"
#include "objects/statusitemlistmodel.h"
#include <tcontentsizer.h>
#include <tpopover.h>
#include <tscrim.h>

struct ConflictResolutionSnapInPrivate {
        GitOperationPtr gitOperation;
        StatusItemListFilterView* statusModel;
};

ConflictResolutionSnapIn::ConflictResolutionSnapIn(GitOperationPtr gitOperation, QWidget* parent) :
    SnapIn(parent),
    ui(new Ui::ConflictResolutionSnapIn) {
    ui->setupUi(this);

    d = new ConflictResolutionSnapInPrivate();
    d->gitOperation = gitOperation;

    ui->titleLabel->setBackButtonShown(true);
    ui->abortTitleLabel->setBackButtonShown(true);
    ui->leftWidget->setFixedWidth(SC_DPI_W(300, this));
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
    ui->stackedWidget->setCurrentWidget(ui->conflictResolutionPage, false);
    ui->doAbortButton->setProperty("type", "destructive");
    new tContentSizer(ui->abortConfirmWidget);

    if (gitOperation.objectCast<Merge>()) {
        ui->abortExplainText->setText(tr("Aborting the conflict resolution at this point will return all files in the repository to the state they were in before you started merging, and will also abort the merge operation. Any conflict resolution will be lost."));
        ui->continueConflictResolutionButton->setText(tr("Continue Merge"));
        ui->continueConflictResolutionButton->setIcon(QIcon::fromTheme("vcs-merge"));
        ui->doAbortButton->setText(tr("Abort Merge"));
        ui->completeButton->setText(tr("Merge"));
        ui->completeButton->setIcon(QIcon::fromTheme("vcs-merge"));
    }

    StatusItemListModel* statusModel = new StatusItemListModel(this);
    statusModel->setStatusItems(gitOperation->repository()->fileStatus());

    d->statusModel = new StatusItemListFilterView(this);
    d->statusModel->setSourceModel(statusModel);
    d->statusModel->setFlagFilters(Repository::StatusItem::Conflicting);
    ui->modifiedFilesEdit->setModel(d->statusModel);
    ui->modifiedFilesEdit->setItemDelegate(new StatusItemListDelegate(this));
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
