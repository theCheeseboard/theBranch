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
#include "objects/libgit/lgrepository.h"
#include "objects/reference.h"
#include "objects/repository.h"
#include <objects/merge.h>
#include <objects/rebase.h>
#include <tcontentsizer.h>
#include <tmessagebox.h>
#include <touchbar/tcompositetouchbar.h>
#include <touchbar/ttouchbarbuttonitem.h>
#include <touchbar/ttouchbardialogalertitem.h>

struct PullSnapInPrivate {
        RepositoryPtr repo;
        GitOperationPtr mergeOperation;

        tCompositeTouchBar* touchBar;
        tTouchBar* pullTouchBar;
        tTouchBarDialogAlertItem* pullDialogAlert;
};

PullSnapIn::PullSnapIn(RepositoryPtr repo, QWidget* parent) :
    SnapIn(parent),
    ui(new Ui::PullSnapIn) {
    ui->setupUi(this);
    d = new PullSnapInPrivate();
    d->repo = repo;

    d->touchBar = new tCompositeTouchBar(this);
    d->pullTouchBar = new tTouchBar(this);
    auto dialogAlertItem = new tTouchBarDialogAlertItem(QStringLiteral("com.vicr123.thebranch.pull.buttons"), "", tr("Cancel"), tr("Pull"), this);
    connect(dialogAlertItem->negativeButton(), &tTouchBarButtonItem::clicked, this, [this] {
        emit ui->titleLabel->backButtonClicked();
    });
    connect(dialogAlertItem->positiveButton(), &tTouchBarButtonItem::clicked, ui->pullButton, &QPushButton::click);
    d->pullTouchBar->addDefaultItem(dialogAlertItem);

    ui->stackedWidget->setCurrentWidget(ui->pullOptionsPage);
    ui->titleLabel->setBackButtonShown(true);
    new tContentSizer(ui->pullOptionsWidget);
    new tContentSizer(ui->pullButton);
    ui->spinner->setFixedSize(QSize(32, 32));

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

    try {
        co_await d->repo->fetch(branch->remoteName(), {}, this->parentPopover()->getInformationRequiredCallback());
    } catch (const QException& ex) {
        ui->stackedWidget->setCurrentWidget(ui->pullOptionsPage);
        co_return;
    }

    // TODO: Merge the repository
    this->setEnabled(false);
    if (ui->rebaseCheckbox->isChecked()) {
        PullRebasePtr rebase(new PullRebase(d->repo, d->repo->head()->asBranch(), branch));
        rebase->setSignature(d->repo->git_repository()->defaultSignature());
        connect(rebase.data(), &Rebase::conflictEncountered, this, [this, rebase] {
            this->parentPopover()->pushSnapIn(new ConflictResolutionSnapIn(rebase));
        });
        connect(rebase.data(), &Rebase::rebaseComplete, this, [this] {
            d->mergeOperation.clear();
            emit done();
        });

        rebase->finaliseOperation();

        // Keep a reference to the operation so it does not get deleted
        d->mergeOperation = rebase;
    } else {
        PullMergePtr merge(new PullMerge(d->repo, branch));
        if (merge->mergeType() == Merge::UpToDate) {
            emit done();
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

tTouchBar* PullSnapIn::touchBar() {
    return d->touchBar;
}

void PullSnapIn::on_stackedWidget_switchingFrame(int index) {
    auto widget = ui->stackedWidget->widget(index);
    if (widget == ui->pullOptionsPage) {
        d->touchBar->setCurrentTouchBar(d->pullTouchBar);
    } else {
        d->touchBar->setCurrentTouchBar(nullptr);
    }
}
