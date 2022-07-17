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
#include "newremotepopover.h"
#include "ui_newremotepopover.h"

#include "objects/errorresponse.h"
#include "objects/repository.h"
#include <tcontentsizer.h>
#include <terrorflash.h>

struct NewRemotePopoverPrivate {
        RepositoryPtr repo;
};

NewRemotePopover::NewRemotePopover(RepositoryPtr repo, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::NewRemotePopover) {
    ui->setupUi(this);
    d = new NewRemotePopoverPrivate();
    d->repo = repo;

    ui->titleLabel->setBackButtonShown(true);
    new tContentSizer(ui->remoteOptionsWidget);
    new tContentSizer(ui->addRemoteButton);
}

NewRemotePopover::~NewRemotePopover() {
    delete d;
    delete ui;
}

void NewRemotePopover::on_titleLabel_backButtonClicked() {
    emit done();
}

void NewRemotePopover::on_addRemoteButton_clicked() {
    if (ui->nameBox->text().isEmpty()) {
        tErrorFlash::flashError(ui->nameContainer);
        return;
    }

    if (ui->urlBox->text().isEmpty()) {
        tErrorFlash::flashError(ui->urlContainer);
        return;
    }

    auto remote = d->repo->addRemote(ui->nameBox->text(), ui->urlBox->text());
    if (!remote) {
        auto error = ErrorResponse::fromCurrentGitError();
        tErrorFlash::flashError(ui->nameContainer, error.description());
        return;
    }

    emit done();
}
