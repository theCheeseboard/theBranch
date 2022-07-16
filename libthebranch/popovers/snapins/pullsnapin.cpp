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
#include "pullsnapin.h"
#include "conflictresolutionsnapin.h"
#include "ui_pullsnapin.h"

#include "../snapinpopover.h"
#include "objects/branchmodel.h"
#include "objects/reference.h"
#include "objects/repository.h"
#include <objects/merge.h>
#include <tcontentsizer.h>
#include <tmessagebox.h>

struct PullSnapInPrivate {
        RepositoryPtr repo;
};

PullSnapIn::PullSnapIn(RepositoryPtr repo, QWidget* parent) :
    SnapIn(parent),
    ui(new Ui::PullSnapIn) {
    ui->setupUi(this);
    d = new PullSnapInPrivate();
    d->repo = repo;

    ui->stackedWidget->setCurrentWidget(ui->pullOptionsPage);
    ui->titleLabel->setBackButtonShown(true);
    new tContentSizer(ui->pullOptionsWidget);
    new tContentSizer(ui->pullButton);
    ui->spinner->setFixedSize(SC_DPI_WT(QSize(32, 32), QSize, this));

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);

    auto* model = new BranchModel();
    model->setBranchFlags(THEBRANCH::RemoteBranches);
    model->setRepository(repo);
    ui->branchBox->setModel(model);

    auto branch = repo->head()->asBranch()->upstream();
    if (branch) {
        auto index = model->index(branch);
        if (index.isValid()) ui->branchBox->setCurrentIndex(index.row());
    }
}

PullSnapIn::~PullSnapIn() {
    delete ui;
}

void PullSnapIn::on_titleLabel_backButtonClicked() {
    emit done();
}

QCoro::Task<> PullSnapIn::on_pullButton_clicked() {
    auto branch = ui->branchBox->currentData(BranchModel::Branch).value<BranchPtr>();
    ui->stackedWidget->setCurrentWidget(ui->pullingPage);

    //    try {
    co_await d->repo->fetch(branch->remoteName());
    //    } catch (const GitRepositoryOutOfDateException& ex) {
    //        ui->stackedWidget->setCurrentWidget(ui->pushFailedPage);
    //        co_return;
    //    }

    // TODO: Merge the repository
    if (ui->rebaseCheckbox->isChecked()) {
    } else {
        PullMergePtr merge(new PullMerge(d->repo, branch));
        if (merge->mergeType() == Merge::UpToDate) {
            emit done();
            tMessageBox* box = new tMessageBox(this->window());
            box->setTitleBarText(tr("Up to date"));
            box->setMessageText(tr("There are no changes to pull."));
            box->setIcon(QMessageBox::Information);
            box->exec(true);
        } else if (merge->mergeType() == Merge::MergeNotPossible) {
            emit done();
            tMessageBox* box = new tMessageBox(this->window());
            box->setTitleBarText(tr("Pull not possible"));

            switch (merge->mergeNotPossibleReason()) {
                case Merge::MergeNotPossibleBecauseHeadDetached:
                    box->setTitleBarText(tr("HEAD is detached"));
                    box->setMessageText(tr("There is no branch to merge onto. Checkout a branch first, and then pull."));
                    break;
                case Merge::MergeNotPossibleBecauseRepositoryNotIdle:
                    box->setTitleBarText(tr("Ongoing operation"));
                    box->setMessageText(tr("There is an ongoing operation in this repository. Complete or abort the ongoing operation, and then pull."));
                    break;
                case Merge::MergeNotPossibleUnknownReason:
                    box->setMessageText(tr("Unable to pull because the merge operation failed."));
                    break;
            }
            box->setIcon(QMessageBox::Critical);
            box->exec(true);
        } else {
            Merge::MergeResult result = merge->performMerge();
            if (result == Merge::MergeFailed) {
                emit done();
                ErrorResponse error = merge->mergeFailureReason();

                QStringList conflicts = error.supplementaryData().value("conflicts").toStringList();

                if (conflicts.length() > 0) {
                    tMessageBox* box = new tMessageBox(this->window());
                    box->setTitleBarText(tr("Unclean Working Directory"));
                    box->setMessageText(tr("To pull these changes, you need to stash your uncommitted changes first."));
                    box->exec(true);
                    co_return;
                }

                tMessageBox* box = new tMessageBox(this->window());
                box->setTitleBarText(tr("Pull failed"));
                box->setMessageText(error.description());
                box->setIcon(QMessageBox::Critical);
                box->exec(true);
            } else if (result == Merge::MergeConflict) {
                this->parentPopover()->pushSnapIn(new ConflictResolutionSnapIn(merge));
                QTimer::singleShot(0, this, &PullSnapIn::done);
            } else {
                emit done();
            }
        }
    }
}
