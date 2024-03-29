#include "repositorystatusbar.h"
#include "ui_repositorystatusbar.h"

#include "objects/branchuihelper.h"
#include "objects/commit.h"
#include "objects/reference.h"
#include "objects/repository.h"
#include "popovers/snapinpopover.h"
#include "popovers/snapins/checkoutsnapin.h"
#include <tmessagebox.h>
#include <tpopover.h>

struct RepositoryStatusBarPrivate {
        RepositoryPtr repository;
};

RepositoryStatusBar::RepositoryStatusBar(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::RepositoryStatusBar) {
    ui->setupUi(this);
    d = new RepositoryStatusBarPrivate();
}

RepositoryStatusBar::~RepositoryStatusBar() {
    delete d;
    delete ui;
}

void RepositoryStatusBar::setRepository(RepositoryPtr repository) {
    if (d->repository) d->repository->disconnect(this);
    d->repository = repository;
    if (d->repository) {
        connect(d->repository.data(), &Repository::repositoryUpdated, this, &RepositoryStatusBar::updateRepository);
        updateRepository();
    }
}

void RepositoryStatusBar::updateRepository() {
    if (!d->repository) return;
    ReferencePtr ref = d->repository->head();
    if (ref) {
        if (auto branch = ref->asBranch()) {
            ui->checkoutButton->setText(branch->name());
        } else if (auto commit = ref->asCommit()) {
            ui->checkoutButton->setText(commit->shortCommitHash());
        } else {
            ui->checkoutButton->setText(ref->shorthand());
        }
    } else {
        ui->checkoutButton->setText(tr("(no HEAD)"));
    }

    switch (d->repository->gitState()) {
        case Repository::GitState::Unknown:
        case Repository::GitState::Idle:
        case Repository::GitState::Bisect:
        case Repository::GitState::ApplyMailbox:
        case Repository::GitState::ApplyMailboxOrRebase:
            ui->conflictButton->setVisible(false);
            break;
        case Repository::GitState::Merge:
        case Repository::GitState::Revert:
        case Repository::GitState::RevertSequence:
        case Repository::GitState::Rebase:
        case Repository::GitState::RebaseInteractive:
        case Repository::GitState::RebaseMerge:
        case Repository::GitState::CherryPick:
        case Repository::GitState::CherryPickSequence:
            ui->conflictButton->setVisible(true);
            break;
    }
}

void RepositoryStatusBar::on_checkoutButton_clicked() {
    SnapInPopover* jp = new SnapInPopover();
    jp->pushSnapIn(new CheckoutSnapIn(d->repository));

    tPopover* popover = new tPopover(jp);
    popover->setPopoverWidth(SC_DPI_W(-200, this));
    popover->setPopoverSide(tPopover::Bottom);
    connect(jp, &SnapInPopover::done, popover, &tPopover::dismiss);
    connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
    connect(popover, &tPopover::dismissed, jp, &SnapInPopover::deleteLater);
    popover->show(this->window());
}

void RepositoryStatusBar::on_conflictButton_clicked() {
    BranchUiHelper::deconflictify(d->repository, this);
}
