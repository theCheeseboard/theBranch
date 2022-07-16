#include "checkoutsnapin.h"
#include "ui_checkoutsnapin.h"

#include "objects/branchmodel.h"
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

    BranchModel* model = new BranchModel();
    model->setBranchFlags(THEBRANCH::LocalBranches);
    model->setRepository(repository);
    ui->branchBox->setModel(model);

    ReferencePtr ref = repository->head();
}

CheckoutSnapIn::~CheckoutSnapIn() {
    delete ui;
    delete d;
}

void CheckoutSnapIn::on_titleLabel_backButtonClicked() {
    emit done();
}

void CheckoutSnapIn::on_checkoutButton_clicked() {
    if (ui->checkoutBranchButton->isChecked()) {
        emit done();

        BranchPtr branch = ui->branchBox->currentData(BranchModel::Branch).value<BranchPtr>();

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
    }
}
