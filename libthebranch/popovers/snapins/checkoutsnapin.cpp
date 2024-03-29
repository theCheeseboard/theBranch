#include "checkoutsnapin.h"
#include "ui_checkoutsnapin.h"

#include "objects/branchmodel.h"
#include "objects/commit.h"
#include "objects/repository.h"
#include <tcontentsizer.h>
#include <tmessagebox.h>

struct CheckoutSnapInPrivate {
        RepositoryPtr repository;
};

CheckoutSnapIn::CheckoutSnapIn(RepositoryPtr repository, QWidget* parent) :
    SnapIn(parent),
    ui(new Ui::CheckoutSnapIn) {
    ui->setupUi(this);
    d = new CheckoutSnapInPrivate();
    d->repository = repository;

    ui->titleLabel->setBackButtonShown(true);
    new tContentSizer(ui->checkoutOptionsWidget);
    new tContentSizer(ui->checkoutButton);

    auto model = new BranchModel();
    model->setBranchFlags(THEBRANCH::LocalBranches);
    model->setRepository(repository);
    ui->branchBox->setModel(model);

    ui->commitBox->setRepository(repository);
    connect(ui->commitBox, &CommitBox::commitChanged, this, &CheckoutSnapIn::updateWidgets);

    this->updateWidgets();
}

CheckoutSnapIn::~CheckoutSnapIn() {
    delete ui;
    delete d;
}

void CheckoutSnapIn::on_titleLabel_backButtonClicked() {
    emit done();
}

void CheckoutSnapIn::on_checkoutButton_clicked() {
    emit done();

    if (ui->checkoutBranchButton->isChecked()) {
        auto branch = ui->branchBox->currentData(BranchModel::Branch).value<BranchPtr>();

        if (CHK_ERR(d->repository->setHeadAndCheckout(branch->toReference()))) {
            QStringList conflicts = error.supplementaryData().value("conflicts").toStringList();

            if (conflicts.length() > 0) {
                auto* box = new tMessageBox(this->window());
                box->setTitleBarText(tr("Unclean Working Directory"));
                box->setMessageText(tr("To checkout this branch, you need to stash your uncommitted changes first."));
                box->exec(true);
                return;
            }

            auto* box = new tMessageBox(this->window());
            box->setTitleBarText(tr("Can't checkout that branch"));
            box->setMessageText(error.description());
            box->setIcon(QMessageBox::Critical);
            box->exec(true);
        }
    } else if (ui->checkoutCommitButton->isChecked()) {
        auto commit = ui->commitBox->commit();
        if (CHK_ERR(d->repository->detachHead(commit))) {
            QStringList conflicts = error.supplementaryData().value("conflicts").toStringList();

            if (conflicts.length() > 0) {
                auto* box = new tMessageBox(this->window());
                box->setTitleBarText(tr("Unclean Working Directory"));
                box->setMessageText(tr("To checkout this commit, you need to stash your uncommitted changes first."));
                box->exec(true);
                return;
            }

            auto* box = new tMessageBox(this->window());
            box->setTitleBarText(tr("Can't checkout that commit"));
            box->setMessageText(error.description());
            box->setIcon(QMessageBox::Critical);
            box->exec(true);
        }
    }
}

void CheckoutSnapIn::on_checkoutCommitButton_toggled(bool checked) {
    if (checked) {
        this->updateWidgets();
        ui->commitBox->setFocus();
    }
}

void CheckoutSnapIn::on_checkoutBranchButton_toggled(bool checked) {
    if (checked) {
        this->updateWidgets();
        ui->branchBox->setFocus();
    }
}

void CheckoutSnapIn::updateWidgets() {
    ui->branchBox->setEnabled(ui->checkoutBranchButton->isChecked());
    ui->commitBox->setEnabled(ui->checkoutCommitButton->isChecked());

    if (ui->checkoutBranchButton->isChecked()) {
        ui->checkoutButton->setEnabled(true);
    } else {
        ui->checkoutButton->setEnabled(!ui->commitBox->commit().isNull());
    }
}
