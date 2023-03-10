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

#include "accounts/accountsmanager.h"
#include "accounts/github/githubaccount.h"
#include "accounts/github/pr/githubcreatepullrequestpopover.h"
#include "branch.h"
#include "branchservices.h"
#include "cherrypick.h"
#include "commit.h"
#include "merge.h"
#include "popovers/newbranchpopover.h"
#include "popovers/snapinpopover.h"
#include "popovers/snapins/cherrypicksnapin.h"
#include "popovers/snapins/mergesnapin.h"
#include "popovers/snapins/rebasesnapin.h"
#include "rebase.h"
#include "reference.h"
#include "remote.h"
#include "repository.h"
#include "stash.h"
#include <QClipboard>
#include <QLocale>
#include <QMenu>
#include <tapplication.h>
#include <tinputdialog.h>
#include <tmessagebox.h>
#include <tpopover.h>

void BranchUiHelper::appendCommitMenu(QMenu* menu, CommitPtr commit, RepositoryPtr repo, QWidget* parent) {
    menu->addSection(tr("For commit %1").arg(QLocale().quoteString(commit->commitHash())));
    menu->addAction(QIcon::fromTheme("vcs-checkout"), tr("Checkout"), parent, [commit, repo, parent] {
        BranchUiHelper::checkoutCommit(repo, commit, parent);
    });
    menu->addSeparator();
    menu->addAction(QIcon::fromTheme("edit-copy"), tr("Copy Commit Message"), parent, [commit] {
        tApplication::clipboard()->setText(commit->commitMessage());
    });
    menu->addAction(QIcon::fromTheme("edit-copy"), tr("Copy Commit Hash"), [commit] {
        tApplication::clipboard()->setText(commit->commitHash());
    });
    menu->addSeparator();
    menu->addAction(QIcon::fromTheme("vcs-tag"), tr("Tag"));
    menu->addAction(QIcon::fromTheme("vcs-cherry-pick"), tr("Cherry Pick"), parent, [parent, commit, repo] {
        QTimer::singleShot(0, parent, [repo, commit, parent] {
            cherryPick(repo, commit, parent);
        });
    });
    menu->addAction(QIcon::fromTheme("vcs-branch-create"), tr("Branch from here"), [parent, commit, repo] {
        BranchUiHelper::branch(repo, commit, parent);
    });
    menu->addAction(QIcon::fromTheme("vcs-revert"), tr("Create Revert Commit"));
    menu->addSeparator();

    auto resetMenu = new QMenu(menu);
    resetMenu->setTitle(tr("Reset to here"));
    resetMenu->addAction(tr("Hard Reset"), [parent, commit, repo]() -> QCoro::Task<> {
        co_await BranchUiHelper::reset(repo, commit, Repository::ResetType::HardReset, parent);
    });
    resetMenu->addAction(tr("Mixed Reset"), [parent, commit, repo]() -> QCoro::Task<> {
        co_await BranchUiHelper::reset(repo, commit, Repository::ResetType::MixedReset, parent);
    });
    resetMenu->addAction(tr("Soft Reset"), [parent, commit, repo]() -> QCoro::Task<> {
        co_await BranchUiHelper::reset(repo, commit, Repository::ResetType::SoftReset, parent);
    });
    menu->addMenu(resetMenu);
}

void BranchUiHelper::appendBranchMenu(QMenu* menu, BranchPtr branch, RepositoryPtr repo, QWidget* parent) {
    QString head;
    if (repo->head()) head = repo->head()->shorthand();

    menu->addSection(tr("For branch %1").arg(QLocale().quoteString(branch->name())));
    menu->addAction(QIcon::fromTheme("vcs-checkout"), tr("Checkout"), parent, [branch, repo, parent] {
        checkoutBranch(repo, branch, parent);
    });
    menu->addSeparator();
    auto merge1 = menu->addAction(QIcon::fromTheme("vcs-merge"), tr("Merge %1 into %2").arg(QLocale().quoteString(branch->name()), QLocale().quoteString(head)), parent, [repo, branch, parent] {
        QTimer::singleShot(0, parent, [repo, branch, parent] {
            merge(repo, branch, parent);
        });
    });                                                                                                                                                           // Branch -> HEAD
    auto merge2 = menu->addAction(QIcon::fromTheme("vcs-merge"), tr("Merge %1 into %2").arg(QLocale().quoteString(head), QLocale().quoteString(branch->name()))); // HEAD -> Branch
    menu->addSeparator();
    auto rebase = menu->addAction(QIcon::fromTheme("vcs-rebase"), tr("Rebase %1 on top of %2").arg(QLocale().quoteString(head), QLocale().quoteString(branch->name())), parent, [repo, branch, parent] {
        BranchUiHelper::rebaseBranch(repo, repo->head()->asBranch(), branch, parent);
    });
    menu->addSeparator();

    auto prBranch = branch;
    if (!branch->isRemoteBranch()) prBranch = branch->upstream();

    if (prBranch && prBranch->isRemoteBranch()) {
        for (auto account : BranchServices::accounts()->accounts()) {
            if (auto gh = qobject_cast<GitHubAccount*>(account)) {
                for (auto remote : repo->remotes()) {
                    if (remote->name() != prBranch->remoteName()) continue;

                    auto slug = remote->slugForAccount(gh);
                    if (slug.isEmpty()) continue;

                    // This remote is a GitHub remote
                    auto branch = repo->head()->asBranch();
                    if (!branch) continue;
                    auto upstreamOfHead = branch->upstream();
                    if (!upstreamOfHead) continue;
                    if (upstreamOfHead->remoteName() != remote->name()) continue;

                    // This branch has an upstream on the same remote
                    menu->addAction(QIcon::fromTheme("vcs-pull-request-create"), tr("Request Pull from %1 into %2").arg(QLocale().quoteString(head), QLocale().quoteString(prBranch->name())), parent, [gh, prBranch, repo, parent, remote] {
                        auto* jp = new GitHubCreatePullRequestPopover(gh, repo->head()->asBranch(), prBranch, remote);
                        auto* popover = new tPopover(jp);
                        popover->setPopoverWidth(SC_DPI_W(-200, parent));
                        popover->setPopoverSide(tPopover::Bottom);
                        connect(jp, &GitHubCreatePullRequestPopover::done, popover, &tPopover::dismiss);
                        connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
                        connect(popover, &tPopover::dismissed, jp, &GitHubCreatePullRequestPopover::deleteLater);
                        popover->show(parent->window());
                    });
                }
            }
        }
    }

    menu->addSeparator();
    menu->addAction(QIcon::fromTheme("edit-rename"), tr("Rename"), parent, [repo, branch, parent] {
        BranchUiHelper::renameBranch(repo, branch, parent);
    });
    menu->addAction(QIcon::fromTheme("vcs-tag"), tr("Tag"));
    menu->addAction(QIcon::fromTheme("vcs-branch-create"), tr("Branch from here"), parent, [repo, branch, parent] {
        BranchUiHelper::branch(repo, branch->lastCommit(), parent);
    });
    menu->addSeparator();
    menu->addAction(QIcon::fromTheme("edit-delete"), tr("Delete"), parent, [repo, branch, parent] {
        BranchUiHelper::deleteBranch(repo, branch, parent);
    });

    if (!repo->head()->asBranch()) {
        merge1->setEnabled(false);
        merge2->setEnabled(false);
        rebase->setEnabled(false);
    }
}

void BranchUiHelper::appendStashMenu(QMenu* menu, StashPtr stash, RepositoryPtr repo, QWidget* parent) {
    menu->addSection(tr("For stash"));
    menu->addAction(QIcon::fromTheme("vcs-stash-pop"), tr("Pop Stash"), parent, [stash, parent]() -> QCoro::Task<> {
        if (!stash->apply()) {
            ErrorResponse error = ErrorResponse::fromCurrentGitError();
            tMessageBox box(parent->window());
            box.setTitleBarText(tr("Couldn't apply stash"));
            box.setMessageText(error.description());
            box.setIcon(QMessageBox::Critical);
            co_await box.presentAsync();
            co_return;
        }

        stash->drop();
    });
    menu->addAction(QIcon::fromTheme("vcs-stash-apply"), tr("Apply Stash"), parent, [stash, parent]() -> QCoro::Task<> {
        if (!stash->apply()) {
            ErrorResponse error = ErrorResponse::fromCurrentGitError();
            tMessageBox box(parent->window());
            box.setTitleBarText(tr("Couldn't apply stash"));
            box.setMessageText(error.description());
            box.setIcon(QMessageBox::Critical);
            co_await box.presentAsync();
        }
    });
    menu->addSeparator();
    menu->addAction(QIcon::fromTheme("list-remove"), tr("Drop Stash"), parent, [stash, parent]() -> QCoro::Task<> {
        tMessageBox box(parent->window());
        box.setTitleBarText(tr("Drop Stash?"));
        box.setMessageText(tr("Do you want to drop the stash %1?").arg(QLocale().quoteString(stash->message())));

        auto* dropButton = box.addButton(tr("Drop Stash"), QMessageBox::DestructiveRole);
        box.addStandardButton(QMessageBox::Cancel);

        auto* pressedButton = co_await box.presentAsync();
        if (pressedButton == dropButton) {
            stash->drop();
        }
    });
}

void BranchUiHelper::appendRemoteMenu(QMenu* menu, RemotePtr remote, RepositoryPtr repo, QWidget* parent) {
    menu->addSection(tr("For remote %1").arg(QLocale().quoteString(remote->name())));
    menu->addAction(QIcon::fromTheme("vcs-pull"), tr("Fetch"), parent, [] {

    });
    menu->addAction(QIcon::fromTheme("list-remove"), tr("Delete Remote"), parent, [parent, remote]() -> QCoro::Task<> {
        auto rm = remote;
        tMessageBox box(parent->window());
        box.setTitleBarText(tr("Delete Remote"));
        box.setMessageText(tr("Do you want to delete %1?").arg(QLocale().quoteString(remote->name())));
        box.setInformativeText(tr("Items on the remote will remain, but you won't be able to push or pull from it unless you add the remote again."));
        auto affirmativeButton = box.addButton(tr("Delete"), QMessageBox::DestructiveRole);
        box.addStandardButton(QMessageBox::Cancel);
        box.setIcon(QMessageBox::Question);
        auto button = co_await box.presentAsync();

        if (button == affirmativeButton) {
            rm->remove();
        }
    });
}

void BranchUiHelper::checkoutBranch(RepositoryPtr repo, BranchPtr branch, QWidget* parent) {
    auto performCheckout = [=] {
        if (CHK_ERR(repo->setHeadAndCheckout(branch->toReference()))) {
            QStringList conflicts = error.supplementaryData().value("conflicts").toStringList();

            if (conflicts.length() > 0) {
                tMessageBox* box = new tMessageBox(parent->window());
                box->setTitleBarText(tr("Unclean Working Directory"));
                box->setMessageText(tr("To checkout this branch, you need to stash or discard your uncommitted changes first."));
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

QCoro::Task<> BranchUiHelper::checkoutCommit(RepositoryPtr repo, CommitPtr commit, QWidget* parent) {
    if (CHK_ERR(repo->detachHead(commit))) {
        QStringList conflicts = error.supplementaryData().value("conflicts").toStringList();

        if (conflicts.length() > 0) {
            tMessageBox box(parent->window());
            box.setTitleBarText(tr("Unclean Working Directory"));
            box.setMessageText(tr("To checkout this commit, you need to stash or discard your uncommitted changes first."));
            co_await box.presentAsync();
            co_return;
        }

        tMessageBox box(parent->window());
        box.setTitleBarText(tr("Can't checkout that commit"));
        box.setMessageText(error.description());
        box.setIcon(QMessageBox::Critical);
        co_await box.presentAsync();
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

void BranchUiHelper::cherryPick(RepositoryPtr repo, CommitPtr commit, QWidget* parent) {
    auto head = repo->head()->shorthand();

    CherryPickPtr cherryPick(new CherryPick(repo, commit));
    SnapInPopover::showSnapInPopover(parent, new CherryPickSnapIn(cherryPick));
}

QCoro::Task<> BranchUiHelper::reset(RepositoryPtr repo, CommitPtr commit, Repository::ResetType resetType, QWidget* parent) {
    tMessageBox box(parent->window());
    tMessageBoxButton* resetButton;
    switch (resetType) {
        case Repository::ResetType::HardReset:
            box.setTitleBarText(tr("Hard Reset?"));
            box.setMessageText(tr("Do you want to hard reset the repository to %1?").arg(QLocale().quoteString(commit->shortCommitHash())));
            box.setInformativeText(tr("The working directory will be updated to reflect the state of the repository at that commit. Any changes will be lost."));
            resetButton = box.addButton(tr("Hard Reset"), QMessageBox::DestructiveRole);
            break;
        case Repository::ResetType::MixedReset:
            box.setTitleBarText(tr("Mixed Reset?"));
            box.setMessageText(tr("Do you want to mixed reset the repository to %1?").arg(QLocale().quoteString(commit->shortCommitHash())));
            box.setInformativeText(tr("The working directory and index will be kept as-is."));
            resetButton = box.addButton(tr("Mixed Reset"), QMessageBox::DestructiveRole);
            break;
        case Repository::ResetType::SoftReset:
            box.setTitleBarText(tr("Soft Reset?"));
            box.setMessageText(tr("Do you want to soft reset the repository to %1?").arg(QLocale().quoteString(commit->shortCommitHash())));
            box.setInformativeText(tr("The working directory will be kept as-is. Any differences will be staged for commit."));
            resetButton = box.addButton(tr("Soft Reset"), QMessageBox::DestructiveRole);
            break;
    }

    box.addStandardButton(QMessageBox::Cancel);

    auto* pressedButton = co_await box.presentAsync();
    if (pressedButton == resetButton) {
        if (CHK_ERR(repo->reset(commit, resetType))) {
            tMessageBox box(parent->window());
            box.setTitleBarText(tr("Couldn't reset the repository"));
            box.setMessageText(error.description());
            box.setIcon(QMessageBox::Critical);
            co_await box.presentAsync();
            co_return;
        }
    }
}

QCoro::Task<> BranchUiHelper::rebaseBranch(RepositoryPtr repo, BranchPtr from, BranchPtr onto, QWidget* parent) {
    RebasePtr rebase(new Rebase(repo, from, onto));
    if (!rebase->isValid()) {
        auto error = ErrorResponse::fromCurrentGitError();

        tMessageBox box(parent->window());
        if (error.error() == ErrorResponse::WorkingDirectoryNotCleanError) {
            box.setTitleBarText(tr("Unclean Working Directory"));
            box.setMessageText(tr("To rebase, you need to stash your uncommitted changes first."));
        } else {
            box.setTitleBarText(tr("Rebase not possible"));
            box.setMessageText(tr("Unable to rebase %1 onto %2.").arg(QLocale().quoteString(from->name()), QLocale().quoteString(onto->name())));
            box.setInformativeText(error.description());
        }
        box.setIcon(QMessageBox::Critical);
        co_await box.presentAsync();
        co_return;
    }
    SnapInPopover::showSnapInPopover(parent, new RebaseSnapIn(rebase));
}

QCoro::Task<> BranchUiHelper::renameBranch(RepositoryPtr repo, BranchPtr branch, QWidget* parent) {
    if (branch->isRemoteBranch()) {
        tMessageBox box(parent->window());
        box.setTitleBarText(tr("Couldn't rename the branch"));
        box.setMessageText(tr("%1 is a remote branch").arg(QLocale().quoteString(branch->name())));
        box.setInformativeText(tr("To rename a remote branch, check it out, delete the remote branch, rename the local branch and push it to the remote."));
        box.setIcon(QMessageBox::Critical);
        co_await box.presentAsync();
        co_return;
    }

    bool ok;
    auto newName = tInputDialog::getText(parent->window(), tr("Rename Branch"), tr("What do you want to rename %1 to?").arg(QLocale().quoteString(branch->name())), QLineEdit::Normal, branch->name(), &ok);
    if (ok) {
        if (CHK_ERR(branch->rename(newName))) {
            tMessageBox box(parent->window());
            box.setTitleBarText(tr("Couldn't rename the branch"));
            box.setMessageText(error.description());
            box.setIcon(QMessageBox::Critical);
            co_await box.presentAsync();
        }
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

QCoro::Task<> BranchUiHelper::discardRepositoryChanges(RepositoryPtr repo, QWidget* parent) {
    tMessageBox box(parent->window());
    box.setTitleBarText(tr("Discard Changes?"));
    box.setMessageText(tr("Do you want to discard all uncommitted changes in the repository?"));
    box.setInformativeText(tr("All uncommitted changes will be gone forever."));
    box.setCheckboxText(tr("Also delete untracked files"));
    tMessageBoxButton* discardButton = box.addButton(tr("Discard Changes"), QMessageBox::DestructiveRole);
    box.addStandardButton(QMessageBox::Cancel);
    box.setIcon(QMessageBox::Question);

    bool checked;
    auto button = co_await box.presentAsync(&checked);
    if (button == discardButton) {
        auto status = repo->fileStatus();
        for (auto item : status) {
            if (!checked && item.flags & Repository::StatusItem::New) continue; // Don't remove any untracked files
            repo->resetFileToHead(item.path);
        }
    }
}
