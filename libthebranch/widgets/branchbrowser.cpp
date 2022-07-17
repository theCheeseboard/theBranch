#include "branchbrowser.h"

#include "objects/branchmodel.h"
#include "objects/merge.h"
#include "objects/reference.h"
#include "objects/repository.h"
#include "popovers/newbranchpopover.h"
#include "popovers/snapinpopover.h"
#include "popovers/snapins/conflictresolutionsnapin.h"
#include <QContextMenuEvent>
#include <QMenu>
#include <tmessagebox.h>
#include <tpopover.h>

struct BranchBrowserPrivate {
        RepositoryPtr repo = nullptr;
        BranchModel* model;
};

BranchBrowser::BranchBrowser(QWidget* parent) :
    QListView{parent} {
    d = new BranchBrowserPrivate();

    d->model = new BranchModel();
    this->setModel(d->model);
    //    this->setItemDelegate(new BranchDelegate(this));
    this->setFrameShape(QFrame::NoFrame);
}

BranchBrowser::~BranchBrowser() {
    delete d;
}

void BranchBrowser::setRepository(RepositoryPtr repo) {
    d->repo = repo;
    d->model->setRepository(repo);
}

void BranchBrowser::contextMenuEvent(QContextMenuEvent* event) {
    if (this->selectedIndexes().isEmpty()) return;

    QModelIndex index = this->selectedIndexes().first();

    QString head = d->repo->head()->shorthand();

    QMenu* menu = new QMenu();
    menu->addSection(tr("For branch %1").arg(QLocale().quoteString(index.data(Qt::DisplayRole).toString())));
    menu->addAction(QIcon::fromTheme("vcs-checkout"), tr("Checkout"), this, [=] {
        BranchPtr branch = index.data(BranchModel::Branch).value<BranchPtr>();

        auto performCheckout = [=] {
            if (CHK_ERR(d->repo->setHeadAndCheckout(branch->toReference()))) {
                QStringList conflicts = error.supplementaryData().value("conflicts").toStringList();

                if (conflicts.length() > 0) {
                    tMessageBox* box = new tMessageBox(this->window());
                    box->setTitleBarText(tr("Unclean Working Directory"));
                    box->setMessageText(tr("To checkout this branch, you need to stash your uncommitted changes first."));
                    box->exec(true);
                    return;
                }

                tMessageBox* box = new tMessageBox(this->window());
                box->setTitleBarText(tr("Can't checkout that branch"));
                box->setMessageText(error.description());
                box->setIcon(QMessageBox::Critical);
                box->exec(true);
            }
        };

        if (branch->isRemoteBranch()) {
            ReferencePtr ref = d->repo->reference("refs/heads/" + branch->localBranchName());
            if (ref) {
                tMessageBox* box = new tMessageBox(this->window());
                box->setTitleBarText(tr("Can't checkout that branch"));
                box->setMessageText(tr("A local branch called %1 already exists.").arg(QLocale().quoteString(branch->localBranchName())));
                box->setIcon(QMessageBox::Critical);
                box->exec(true);
            } else {
                tMessageBox* box = new tMessageBox(this->window());
                box->setTitleBarText(tr("Checkout Remote Branch"));
                box->setMessageText(tr("Do you want to checkout this remote branch?"));
                box->setInformativeText(tr("A new branch, %1, will be created and checked out.")
                                            .arg(QLocale().quoteString(branch->localBranchName())));
                box->setIcon(QMessageBox::Question);
                tMessageBoxButton* checkoutButton = box->addButton(tr("Checkout"), QMessageBox::AcceptRole);
                connect(checkoutButton, &tMessageBoxButton::buttonPressed, this, performCheckout);
                box->addStandardButton(QMessageBox::Cancel);
                box->exec(true);
            }
        } else {
            performCheckout();
        }
    });
    menu->addSeparator();
    menu->addAction(QIcon::fromTheme("vcs-merge"), tr("Merge %1 into %2").arg(QLocale().quoteString(index.data(Qt::DisplayRole).toString()), QLocale().quoteString(head)), [=] {
        QTimer::singleShot(0, [=] {
            MergePtr merge(new Merge(d->repo, index.data(BranchModel::Branch).value<BranchPtr>()));
            if (merge->mergeType() == Merge::UpToDate) {
                tMessageBox* box = new tMessageBox(this->window());
                box->setTitleBarText(tr("Up to date"));
                box->setMessageText(tr("There are no changes to merge from %1.").arg(QLocale().quoteString(index.data(Qt::DisplayRole).toString())));
                box->setIcon(QMessageBox::Information);
                box->exec(true);
            } else if (merge->mergeType() == Merge::MergeNotPossible) {
                tMessageBox* box = new tMessageBox(this->window());
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
                        box->setMessageText(tr("Unable to merge from %1 into %2.").arg(QLocale().quoteString(index.data(Qt::DisplayRole).toString()), QLocale().quoteString(head)));
                        break;
                }
                box->setIcon(QMessageBox::Critical);
                box->exec(true);
            } else {
                tMessageBoxButton* affirmativeButton;
                tMessageBox* box = new tMessageBox(this->window());
                box->setTitleBarText(tr("Merge"));
                if (merge->mergeType() == Merge::FastForward) {
                    box->setMessageText(tr("Do you want to perform a fast-forward merge from %1?").arg(QLocale().quoteString(index.data(Qt::DisplayRole).toString())));
                    affirmativeButton = box->addButton(tr("Fast-Forward"), QMessageBox::AcceptRole);
                } else {
                    box->setMessageText(tr("Do you want to create a merge commit from %1?").arg(QLocale().quoteString(index.data(Qt::DisplayRole).toString())));
                    affirmativeButton = box->addButton(tr("Merge"), QMessageBox::AcceptRole);
                }

                connect(affirmativeButton, &tMessageBoxButton::buttonPressed, this, [=] {
                    Merge::MergeResult result = merge->performMerge();
                    if (result == Merge::MergeFailed) {
                        ErrorResponse error = merge->mergeFailureReason();

                        QStringList conflicts = error.supplementaryData().value("conflicts").toStringList();

                        if (conflicts.length() > 0) {
                            tMessageBox* box = new tMessageBox(this->window());
                            box->setTitleBarText(tr("Unclean Working Directory"));
                            box->setMessageText(tr("To perform a merge, you need to stash your uncommitted changes first."));
                            box->exec(true);
                            return;
                        }

                        tMessageBox* box = new tMessageBox(this->window());
                        box->setTitleBarText(tr("Merge failed"));
                        box->setMessageText(error.description());
                        box->setIcon(QMessageBox::Critical);
                        box->exec(true);
                    } else if (result == Merge::MergeConflict) {
                        SnapInPopover* jp = new SnapInPopover();

                        tPopover* popover = new tPopover(jp);
                        popover->setPopoverWidth(SC_DPI_W(-200, this));
                        popover->setPopoverSide(tPopover::Bottom);
                        connect(jp, &SnapInPopover::done, popover, &tPopover::dismiss);
                        connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
                        connect(popover, &tPopover::dismissed, jp, &SnapInPopover::deleteLater);
                        popover->show(this->window());

                        jp->pushSnapIn(new ConflictResolutionSnapIn(merge));
                    }
                });
                box->setInformativeText(tr("All changes from %1 will be merged into the current branch.").arg(QLocale().quoteString(index.data(Qt::DisplayRole).toString())));
                box->addStandardButton(QMessageBox::Cancel);
                box->setIcon(QMessageBox::Question);
                box->exec(true);
            }
        });
    });                                                                                                                                                                     // Branch -> HEAD
    menu->addAction(QIcon::fromTheme("vcs-merge"), tr("Merge %1 into %2").arg(QLocale().quoteString(head), QLocale().quoteString(index.data(Qt::DisplayRole).toString()))); // HEAD -> Branch
    menu->addSeparator();
    menu->addAction(QIcon::fromTheme("edit-rename"), tr("Rename"));
    menu->addAction(QIcon::fromTheme("vcs-tag"), tr("Tag"));
    menu->addAction(QIcon::fromTheme("vcs-branch-create"), tr("Branch from here"), this, [this, index] {
        BranchPtr branch = index.data(BranchModel::Branch).value<BranchPtr>();
        auto* jp = new NewBranchPopover(d->repo, branch->lastCommit());
        tPopover* popover = new tPopover(jp);
        popover->setPopoverWidth(SC_DPI_W(-200, this));
        popover->setPopoverSide(tPopover::Bottom);
        connect(jp, &NewBranchPopover::done, popover, &tPopover::dismiss);
        connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
        connect(popover, &tPopover::dismissed, jp, &NewBranchPopover::deleteLater);
        popover->show(this->window());
    });
    menu->addSeparator();
    menu->addAction(QIcon::fromTheme("edit-delete"), tr("Delete"), this, [index, this]() -> QCoro::Task<> {
        // TODO: Use a snapin or popover

        BranchPtr branch = index.data(BranchModel::Branch).value<BranchPtr>();
        if (branch->toReference()->shorthand() == d->repo->head()->shorthand()) {
            tMessageBox box(this->window());
            box.setTitleBarText(tr("Can't delete that branch"));
            box.setMessageText(tr("The selected branch is the current branch."));
            box.setInformativeText(tr("To delete the branch, checkout a different branch first."));
            co_await box.presentAsync();
            co_return;
        }

        tMessageBox box(this->window());
        box.setTitleBarText(tr("Delete branch?"));
        box.setMessageText(tr("Do you want to delete the branch %1?").arg(QLocale().quoteString(index.data(Qt::DisplayRole).toString())));
        tMessageBoxButton* deleteButton = box.addButton(tr("Delete Branch"), QMessageBox::DestructiveRole);
        box.addStandardButton(QMessageBox::Cancel);
        box.setIcon(QMessageBox::Question);

        auto button = co_await box.presentAsync();
        if (button == deleteButton) {
            if (branch->isRemoteBranch()) {
                try {
                    co_await branch->deleteRemoteBranch();
                } catch (QException& ex) {
                    tMessageBox* box = new tMessageBox(this->window());
                    box->setTitleBarText(tr("Can't delete that branch"));
                    box->setMessageText(tr("Failed to delete the remote branch"));
                    box->exec(true);
                    co_return;
                }
            }

            if (CHK_ERR(branch->deleteBranch())) {
                tMessageBox box(this->window());
                box.setTitleBarText(tr("Can't delete that branch"));
                box.setMessageText(error.description());
                co_await box.presentAsync();
            }
        }
    });
    //    connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
    menu->popup(event->globalPos());
}
