#include "branchactionsnapin.h"
#include "ui_branchactionsnapin.h"

#include "../snapinpopover.h"
#include "mergesnapin.h"
#include "objects/branchmodel.h"
#include "objects/merge.h"
#include "objects/rebase.h"
#include "objects/reference.h"
#include "objects/repository.h"
#include "rebasesnapin.h"
#include <tcontentsizer.h>
#include <ticon.h>

struct BranchActionSnapInPrivate {
        RepositoryPtr repo;
        BranchActionSnapIn::BranchAction action;
};

BranchActionSnapIn::BranchActionSnapIn(RepositoryPtr repo, BranchAction action, QWidget* parent) :
    SnapIn(parent),
    ui(new Ui::BranchActionSnapIn) {
    ui->setupUi(this);
    d = new BranchActionSnapInPrivate();

    d->repo = repo;
    d->action = action;

    new tContentSizer(ui->optionsWidget);
    new tContentSizer(ui->performActionButton);

    switch (action) {
        case BranchAction::Merge:
            ui->titleLabel->setText(tr("Merge"));
            ui->optionsLabel->setText(tr("Merge Options"));
            ui->commitLabel->setText(tr("Merge:"));
            ui->performActionButton->setText(tr("Merge"));
            ui->performActionButton->setIcon(tIcon::fromTheme("vcs-merge"));
            break;
        case BranchAction::Rebase:
            ui->titleLabel->setText(tr("Rebase"));
            ui->optionsLabel->setText(tr("Rebase Options"));
            ui->commitLabel->setText(tr("Rebase:"));
            ui->performActionButton->setText(tr("Rebase"));
            ui->performActionButton->setIcon(tIcon::fromTheme("vcs-rebase"));
            break;
    }

    BranchModel* model = new BranchModel();
    model->setBranchFlags(THEBRANCH::AllBranches);
    model->setRepository(repo);
    ui->branchBox->setModel(model);
}

BranchActionSnapIn::~BranchActionSnapIn() {
    delete ui;
    delete d;
}

void BranchActionSnapIn::on_performActionButton_clicked() {
    auto branch = ui->branchBox->currentData(BranchModel::Branch).value<BranchPtr>();

    switch (d->action) {
        case BranchAction::Merge:
            {
                MergePtr merge(new Merge(d->repo, branch));
                this->parentPopover()->pushSnapIn(new MergeSnapIn(merge));
                emit done();
                break;
            }
        case BranchAction::Rebase:
            {
                RebasePtr rebase(new Rebase(d->repo, d->repo->head()->asBranch(), branch));
                this->parentPopover()->pushSnapIn(new RebaseSnapIn(rebase));
                emit done();
                break;
            }
    }
}

void BranchActionSnapIn::on_titleLabel_backButtonClicked() {
    emit done();
}
