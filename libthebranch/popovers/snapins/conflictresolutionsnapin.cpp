#include "conflictresolutionsnapin.h"
#include "ui_conflictresolutionsnapin.h"

#include "objects/gitoperation.h"
#include "objects/merge.h"
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
    ui->leftWidget->setFixedWidth(SC_DPI_W(300, this));
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
    ui->stackedWidget->setCurrentWidget(ui->conflictResolutionPage, false);
    ui->conflictResolutionStack->setCurrentAnimation(tStackedWidget::Lift);
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
    for (auto resolver : d->conflictResolutionWidgets) {
        if (!resolver->isConflictResolutionCompleted()) resolutionCompleted = false;
    }

    ui->completeButton->setEnabled(resolutionCompleted);
}
