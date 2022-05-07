#include "branchbrowser.h"

#include "objects/branchmodel.h"
#include "objects/merge.h"
#include "objects/reference.h"
#include "objects/repository.h"
#include <QContextMenuEvent>
#include <QMenu>
#include <tmessagebox.h>

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
    menu->addAction(tr("Checkout"), this, [=] {
        BranchPtr branch = index.data(BranchModel::Branch).value<BranchPtr>();
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
    });
    menu->addSeparator();
    menu->addAction(tr("Merge %1 into %2").arg(QLocale().quoteString(index.data(Qt::DisplayRole).toString()), QLocale().quoteString(head)), [=] {
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
                tMessageBoxButton* affirmativeButton = box->addButton(tr("Merge"), QMessageBox::AcceptRole);
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
                }
            });
            box->setInformativeText(tr("All changes from %1 will be merged into the current branch.").arg(QLocale().quoteString(index.data(Qt::DisplayRole).toString())));
            box->addStandardButton(QMessageBox::Cancel);
            box->setIcon(QMessageBox::Question);
            box->exec(true);
        }
    });                                                                                                                                      // Branch -> HEAD
    menu->addAction(tr("Merge %1 into %2").arg(QLocale().quoteString(head), QLocale().quoteString(index.data(Qt::DisplayRole).toString()))); // HEAD -> Branch
    menu->addSeparator();
    menu->addAction(QIcon::fromTheme("edit-rename"), tr("Rename"));
    menu->addAction(tr("Tag"));
    menu->addAction(tr("Branch from here"));
    menu->addSeparator();
    menu->addAction(QIcon::fromTheme("edit-delete"), tr("Delete"));
    connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
    menu->popup(event->globalPos());
}
