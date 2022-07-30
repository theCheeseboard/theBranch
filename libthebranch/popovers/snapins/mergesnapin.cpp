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
#include "mergesnapin.h"
#include "ui_mergesnapin.h"

#include "../snapinpopover.h"
#include "conflictresolutionsnapin.h"
#include "objects/branch.h"
#include "objects/merge.h"
#include <tcontentsizer.h>

struct MergeSnapInPrivate {
        MergePtr merge;
};

MergeSnapIn::MergeSnapIn(MergePtr merge, QWidget* parent) :
    SnapIn(parent),
    ui(new Ui::MergeSnapIn) {
    ui->setupUi(this);
    d = new MergeSnapInPrivate();
    d->merge = merge;

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
    ui->titleLabel->setBackButtonShown(true);
    ui->titleLabel_2->setBackButtonShown(true);
    new tContentSizer(ui->mergeOptionsWidget);
    new tContentSizer(ui->mergeButton);
    new tContentSizer(ui->mergeUnavailableWidget);
    new tContentSizer(ui->mergeUnavailableButton);

    if (merge->mergeType() == Merge::FastForward) {
        ui->mergeDescription->setText(tr("Do you want to perform a fast-forward merge from %1?").arg(QLocale().quoteString(merge->fromBranch()->name())));
        ui->mergeButton->setText(tr("Fast-Forward"));
    } else {
        ui->mergeDescription->setText(tr("Do you want to create a merge commit from %1?").arg(QLocale().quoteString(merge->fromBranch()->name())));
        ui->mergeButton->setText(tr("Merge"));
    }

    ui->mergeInformation->setText(tr("All changes from %1 will be merged into the current branch.").arg(QLocale().quoteString(merge->fromBranch()->name())));
}

MergeSnapIn::~MergeSnapIn() {
    delete d;
    delete ui;
}

void MergeSnapIn::on_titleLabel_backButtonClicked() {
    emit done();
}

void MergeSnapIn::on_mergeButton_clicked() {
    Merge::MergeResult result = d->merge->performMerge();
    if (result == Merge::MergeFailed) {
        ErrorResponse error = d->merge->mergeFailureReason();

        QStringList conflicts = error.supplementaryData().value("conflicts").toStringList();

        if (conflicts.length() > 0) {
            ui->mergeUnavailableTitle->setText(tr("Unclean Working Directory"));
            ui->mergeUnavailableText->setText(tr("To perform a merge, you need to stash your uncommitted changes first."));
            return;
        } else {
            ui->mergeUnavailableTitle->setText(tr("Merge failed"));
            ui->mergeUnavailableText->setText(error.description());
        }

        ui->stackedWidget->setCurrentWidget(ui->mergeUnavailablePage);
    } else if (result == Merge::MergeConflict) {
        this->parentPopover()->pushSnapIn(new ConflictResolutionSnapIn(d->merge));
        emit done();
    } else {
        emit done();
    }
}

void MergeSnapIn::on_mergeUnavailableButton_clicked() {
    emit done();
}

void MergeSnapIn::on_titleLabel_2_backButtonClicked() {
    ui->stackedWidget->setCurrentWidget(ui->mergePage);
}
