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
#include "rebasesnapin.h"
#include "ui_rebasesnapin.h"

#include "conflictresolutionsnapin.h"
#include "objects/branch.h"
#include "objects/libgit/lgrepository.h"
#include "objects/rebase.h"
#include "objects/repository.h"
#include "popovers/snapinpopover.h"

#include <tcontentsizer.h>

struct RebaseSnapInPrivate {
        RebasePtr rebase;
};

RebaseSnapIn::RebaseSnapIn(RebasePtr rebase, QWidget* parent) :
    SnapIn(parent),
    ui(new Ui::RebaseSnapIn) {
    ui->setupUi(this);

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
    ui->titleLabel->setBackButtonShown(true);
    ui->titleLabel_2->setBackButtonShown(true);
    new tContentSizer(ui->rebaseOptionsWidget);
    new tContentSizer(ui->rebaseButton);
    new tContentSizer(ui->mergeUnavailableWidget);
    new tContentSizer(ui->mergeUnavailableButton);

    d = new RebaseSnapInPrivate();
    d->rebase = rebase;

    ui->rebaseDescription->setText(tr("Do you want to rebase %1 onto %2?").arg(QLocale().quoteString(d->rebase->from()->name()), QLocale().quoteString(d->rebase->onto()->name())));
    ui->rebaseInformation->setText(tr("%n commits from %1 will be replayed onto %2", nullptr, d->rebase->operationCount()).arg(QLocale().quoteString(d->rebase->from()->name()), QLocale().quoteString(d->rebase->onto()->name())));
}

RebaseSnapIn::~RebaseSnapIn() {
    delete d;
    delete ui;
}

void RebaseSnapIn::on_titleLabel_backButtonClicked() {
    emit done();
}

void RebaseSnapIn::on_rebaseButton_clicked() {
    d->rebase->setSignature(d->rebase->repository()->git_repository()->defaultSignature());
    connect(d->rebase.data(), &Rebase::conflictEncountered, this, [this] {
        this->parentPopover()->pushSnapIn(new ConflictResolutionSnapIn(d->rebase));
    });
    connect(d->rebase.data(), &Rebase::rebaseComplete, this, [this] {
        emit done();
    });

    d->rebase->finaliseOperation();
}
