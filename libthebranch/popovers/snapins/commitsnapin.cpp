#include "commitsnapin.h"
#include "popovers/snapinpopover.h"
#include "pushsnapin.h"
#include "ui_commitsnapin.h"

#include "objects/libgit/lgcommit.h"
#include "objects/libgit/lgindex.h"
#include "objects/libgit/lgreference.h"
#include "objects/libgit/lgrepository.h"
#include "objects/libgit/lgsignature.h"
#include "objects/statusitemlistmodel.h"
#include <tcontentsizer.h>
#include <terrorflash.h>

struct CommitSnapInPrivate {
        RepositoryPtr repository;
        StatusItemListModel* statusModel;
        StatusItemListFilterView* statusModelFilter;

        LGSignaturePtr signature;
};

CommitSnapIn::CommitSnapIn(RepositoryPtr repository, QWidget* parent) :
    SnapIn(parent),
    ui(new Ui::CommitSnapIn) {
    ui->setupUi(this);
    d = new CommitSnapInPrivate();
    d->repository = repository;

    ui->titleLabel->setBackButtonShown(true);
    ui->titleLabel_2->setBackButtonShown(true);
    ui->leftWidget->setFixedWidth(SC_DPI_W(300, this));
    libContemporaryCommon::fixateHeight(ui->commitMessageEdit, [=] {
        return SC_DPI_W(100, this);
    });
    new tContentSizer(ui->signatureWidget);

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);

    d->statusModel = new StatusItemListModel(this);
    d->statusModel->setStatusItems(repository->fileStatus());

    d->statusModelFilter = new StatusItemListFilterView(this);
    d->statusModelFilter->setSourceModel(d->statusModel);
    d->statusModelFilter->setFlagFilters(Repository::StatusItem::Modified | Repository::StatusItem::Renamed | Repository::StatusItem::Deleted | Repository::StatusItem::Conflicting);
    ui->modifiedFilesEdit->setModel(d->statusModelFilter);
    ui->modifiedFilesEdit->setItemDelegate(new StatusItemListDelegate(this));

    connect(d->statusModel, &StatusItemListModel::checkedItemsChanged, this, [=] {
        int count = d->statusModel->checkedItems().count();
        ui->commitButton->setText(tr("Commit %n Files", nullptr, count));
        ui->commitButton->setEnabled(count != 0);
    });

    int count = d->statusModel->checkedItems().count();
    ui->commitButton->setText(tr("Commit %n Files", nullptr, count));
    ui->commitButton->setEnabled(count != 0);
}

CommitSnapIn::~CommitSnapIn() {
    delete d;
    delete ui;
}

void CommitSnapIn::on_titleLabel_backButtonClicked() {
    emit done();
}

void CommitSnapIn::on_commitButton_clicked() {
    if (ui->commitMessageEdit->toPlainText().isEmpty()) {
        tErrorFlash::flashError(ui->commitMessageEdit);
        return;
    }

    LGRepositoryPtr repo = d->repository->git_repository();
    LGSignaturePtr sig = repo->defaultSignature();
    if (!sig) {
        ui->stackedWidget->setCurrentWidget(ui->finaliseCommitPage);
    } else {
        d->signature = sig;
        performCommit();
    }
}

void CommitSnapIn::performCommit() {
    LGRepositoryPtr repo = d->repository->git_repository();
    LGSignaturePtr sig = d->signature;
    LGReferencePtr head = repo->head();

    // Create a commit on the existing HEAD
    LGIndexPtr index = repo->index();

    for (QModelIndex selected : d->statusModel->checkedItems()) {
        QString pathspec = selected.data(StatusItemListModel::PathRole).toString();
        if (!index->addByPath(pathspec)) {
            ui->stackedWidget->setCurrentWidget(ui->commitPage);
            ErrorResponse err = ErrorResponse::fromCurrentGitError();
            tErrorFlash::flashError(ui->commitMessageEdit, err.description());
            return;
        }
    }

    if (!index->write()) {
        ui->stackedWidget->setCurrentWidget(ui->commitPage);
        ErrorResponse err = ErrorResponse::fromCurrentGitError();
        tErrorFlash::flashError(ui->commitMessageEdit, err.description());
        return;
    }

    repo->commit(ui->commitMessageEdit->toPlainText(), sig);

    if (ui->pushBox->isChecked()) {
        this->parentPopover()->pushSnapIn(new PushSnapIn(d->repository));
    }
    emit done();
}

void CommitSnapIn::on_titleLabel_2_backButtonClicked() {
    ui->stackedWidget->setCurrentWidget(ui->commitPage);
}

void CommitSnapIn::on_commitButton_2_clicked() {
    if (ui->nameBox->text().isEmpty()) {
        tErrorFlash::flashError(ui->nameBox);
        return;
    }

    if (ui->emailBox->text().isEmpty()) {
        tErrorFlash::flashError(ui->emailBox);
        return;
    }

    if (ui->saveSignatureBox->isChecked()) {
        // TODO: Save the user information in the Git config
    }

    LGRepositoryPtr repo = d->repository->git_repository();
    d->signature = LGSignature::signatureForNow(ui->nameBox->text(), ui->emailBox->text());
    performCommit();
}
