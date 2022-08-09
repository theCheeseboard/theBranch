/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2022 Victor Tran
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * *************************************/
#include "newbranchpopover.h"
#include "ui_newbranchpopover.h"

#include "objects/repository.h"
#include <tcontentsizer.h>
#include <terrorflash.h>

struct NewBranchPopoverPrivate {
    RepositoryPtr repo;
    CommitPtr commit;
};

NewBranchPopover::NewBranchPopover(RepositoryPtr repo, CommitPtr commit, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::NewBranchPopover) {
    ui->setupUi(this);
    d = new NewBranchPopoverPrivate();
    d->repo = repo;
    d->commit = commit;

    ui->titleLabel->setBackButtonShown(true);
    new tContentSizer(ui->branchOptionsWidget);
    new tContentSizer(ui->branchButton);
}

NewBranchPopover::~NewBranchPopover() {
    delete d;
    delete ui;
}

void NewBranchPopover::on_titleLabel_backButtonClicked() {
    emit done();
}

void NewBranchPopover::on_branchButton_clicked() {
    if (ui->nameBox->text().isEmpty()) {
        tErrorFlash::flashError(ui->nameContainer);
        return;
    }

    auto branch = d->repo->createBranch(ui->nameBox->text(), d->commit);
    if (!branch) {
        auto error = ErrorResponse::fromCurrentGitError();
        tErrorFlash::flashError(ui->nameContainer, error.description());
        return;
    }

    if (ui->checkoutBox->isChecked()) {
        d->repo->setHeadAndCheckout(branch->toReference());
    }

    emit done();
}
