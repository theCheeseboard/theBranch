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
#include "branchuihelper.h"

#include "branch.h"
#include "branchmodel.h"
#include "merge.h"
#include "popovers/newbranchpopover.h"
#include "popovers/snapinpopover.h"
#include "popovers/snapins/conflictresolutionsnapin.h"
#include "popovers/snapins/mergesnapin.h"
#include "reference.h"
#include "repository.h"
#include <QLocale>
#include <QMenu>
#include <tmessagebox.h>
#include <tpopover.h>

void BranchUiHelper::appendBranchMenu(QMenu* menu, BranchPtr branch, RepositoryPtr repo, QWidget* parent) {
    auto head = repo->head()->shorthand();

    menu->addSection(tr("For branch %1").arg(QLocale().quoteString(branch->name())));
    menu->addAction(QIcon::fromTheme("vcs-checkout"), tr("Checkout"), parent, [branch, repo, parent] {
        checkoutBranch(repo, branch, parent);
    });
    menu->addSeparator();
    menu->addAction(QIcon::fromTheme("vcs-merge"), tr("Merge %1 into %2").arg(QLocale().quoteString(branch->name()), QLocale().quoteString(head)), [repo, branch, parent] {
        QTimer::singleShot(0, [repo, branch, parent] {
            merge(repo, branch, parent);
        });
    });                                                                                                                                             // Branch -> HEAD
    menu->addAction(QIcon::fromTheme("vcs-merge"), tr("Merge %1 into %2").arg(QLocale().quoteString(head), QLocale().quoteString(branch->name()))); // HEAD -> Branch
    menu->addSeparator();
    menu->addAction(QIcon::fromTheme("edit-rename"), tr("Rename"));
    menu->addAction(QIcon::fromTheme("vcs-tag"), tr("Tag"));
    menu->addAction(QIcon::fromTheme("vcs-branch-create"), tr("Branch from here"), parent, [repo, branch, parent] {
        BranchUiHelper::branch(repo, branch->lastCommit(), parent);
    });
    menu->addSeparator();
    menu->addAction(QIcon::fromTheme("edit-delete"), tr("Delete"), parent, [repo, branch, parent] {
        BranchUiHelper::deleteBranch(repo, branch, parent);
    });
}

void BranchUiHelper::checkoutBranch(RepositoryPtr repo, BranchPtr branch, QWidget* parent) {
    auto performCheckout = [=] {
        if (CHK_ERR(repo->setHeadAndCheckout(branch->toReference()))) {
            QStringList conflicts = error.supplementaryData().value("conflicts").toStringList();

            if (conflicts.length() > 0) {
                tMessageBox* box = new tMessageBox(parent->window());
                box->setTitleBarText(tr("Unclean Working Directory"));
                box->setMessageText(tr("To checkout this branch, you need to stash your uncommitted changes first."));
                box->exec(true);
                return;
            }

            tMessageBox* box = new tMessageBox(parent->window());
            box->setTitleBarText(tr("Can't checkout that branch"));
            box->setMessageText(error.description());
            box->setIcon(QMessageBox::Critical);
            box->exec(true);
        }
    };

    if (branch->isRemoteBranch()) {
        ReferencePtr ref = repo->reference("refs/heads/" + branch->localBranchName());
        if (ref) {
            tMessageBox* box = new tMessageBox(parent->window());
            box->setTitleBarText(tr("Can't checkout that branch"));
            box->setMessageText(tr("A local branch called %1 already exists.").arg(QLocale().quoteString(branch->localBranchName())));
            box->setIcon(QMessageBox::Critical);
            box->exec(true);
        } else {
            tMessageBox* box = new tMessageBox(parent->window());
            box->setTitleBarText(tr("Checkout Remote Branch"));
            box->setMessageText(tr("Do you want to checkout this remote branch?"));
            box->setInformativeText(tr("A new branch, %1, will be created and checked out.")
                                        .arg(QLocale().quoteString(branch->localBranchName())));
            box->setIcon(QMessageBox::Question);
            tMessageBoxButton* checkoutButton = box->addButton(tr("Checkout"), QMessageBox::AcceptRole);
            connect(checkoutButton, &tMessageBoxButton::buttonPressed, parent, performCheckout);
            box->addStandardButton(QMessageBox::Cancel);
            box->exec(true);
        }
    } else {
        performCheckout();
    }
}

void BranchUiHelper::merge(RepositoryPtr repo, BranchPtr branch, QWidget* parent) {
    auto head = repo->head()->shorthand();

    MergePtr merge(new Merge(repo, branch));
    if (merge->mergeType() == Merge::UpToDate) {
        tMessageBox* box = new tMessageBox(parent->window());
        box->setTitleBarText(tr("Up to date"));
        box->setMessageText(tr("There are no changes to merge from %1.").arg(QLocale().quoteString(branch->name())));
        box->setIcon(QMessageBox::Information);
        box->exec(true);
    } else if (merge->mergeType() == Merge::MergeNotPossible) {
        tMessageBox* box = new tMessageBox(parent->window());
        box->setTitleBarText(tr("Merge not possible"));

        switch (merge->mergeNotPossibleReason()) {
            case Merge::MergeNotPossibleBecauseHeadDetached:
                box->setTitleBarText(tr("HEAD is detached"));
                box->setMessageText(tr("There is no branch to merge onto. Checkout a branch first, and then merge your changes."));
                break;
            case Merge::MergeNotPossibleBecauseRepositoryNotIdle:
                box->setTitleBarText(tr("Ongoing operation"));
                box->setMessageText(tr("There is an ongoing operation in this repository. Complete or abort the ongoing operation, and then merge your changes."));
                break;
            case Merge::MergeNotPossibleUnknownReason:
                box->setMessageText(tr("Unable to merge from %1 into %2.").arg(QLocale().quoteString(branch->name()), QLocale().quoteString(head)));
                break;
        }
        box->setIcon(QMessageBox::Critical);
        box->exec(true);
    } else {
        SnapInPopover::showSnapInPopover(parent, new MergeSnapIn(merge));
    }
}

void BranchUiHelper::branch(RepositoryPtr repo, CommitPtr commit, QWidget* parent) {
    auto* jp = new NewBranchPopover(repo, commit);
    auto* popover = new tPopover(jp);
    popover->setPopoverWidth(SC_DPI_W(-200, parent));
    popover->setPopoverSide(tPopover::Bottom);
    connect(jp, &NewBranchPopover::done, popover, &tPopover::dismiss);
    connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
    connect(popover, &tPopover::dismissed, jp, &NewBranchPopover::deleteLater);
    popover->show(parent->window());
}

QCoro::Task<> BranchUiHelper::deleteBranch(RepositoryPtr repo, BranchPtr branch, QWidget* parent) {
    // TODO: Use a snapin or popover

    if (branch->toReference()->shorthand() == repo->head()->shorthand()) {
        tMessageBox box(parent->window());
        box.setTitleBarText(tr("Can't delete that branch"));
        box.setMessageText(tr("The selected branch is the current branch."));
        box.setInformativeText(tr("To delete the branch, checkout a different branch first."));
        co_await box.presentAsync();
        co_return;
    }

    tMessageBox box(parent->window());
    box.setTitleBarText(tr("Delete branch?"));
    box.setMessageText(tr("Do you want to delete the branch %1?").arg(QLocale().quoteString(branch->name())));
    tMessageBoxButton* deleteButton = box.addButton(tr("Delete Branch"), QMessageBox::DestructiveRole);
    box.addStandardButton(QMessageBox::Cancel);
    box.setIcon(QMessageBox::Question);

    auto button = co_await box.presentAsync();
    if (button == deleteButton) {
        if (branch->isRemoteBranch()) {
            try {
                co_await branch->deleteRemoteBranch();
            } catch (QException& ex) {
                tMessageBox* box = new tMessageBox(parent->window());
                box->setTitleBarText(tr("Can't delete that branch"));
                box->setMessageText(tr("Failed to delete the remote branch"));
                box->exec(true);
                co_return;
            }
        }

        if (CHK_ERR(branch->deleteBranch())) {
            tMessageBox box(parent->window());
            box.setTitleBarText(tr("Can't delete that branch"));
            box.setMessageText(error.description());
            co_await box.presentAsync();
        }
    }
}
