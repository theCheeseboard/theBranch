#include "branchbrowser.h"

#include "objects/branchmodel.h"
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

    QMenu* menu = new QMenu();
    menu->addSection(tr("For branch %1").arg(QLocale().quoteString(index.data(Qt::DisplayRole).toString())));
    menu->addAction(tr("Checkout"), this, [=] {
        tMessageBox* box = new tMessageBox(this->window());
        box->setTitleBarText(tr("Checkout this branch?"));
        box->setMessageText(tr("Do you want to checkout %1?").arg(QLocale().quoteString(index.data(Qt::DisplayRole).toString())));
        box->setInformativeText(tr("All files in the working directory will switch to reflect the state of the repository at this point."));
        box->addButton(tr("Checkout"), QMessageBox::AcceptRole);
        box->addStandardButton(QMessageBox::Cancel);
        box->exec(true);
    });
    menu->addSeparator();
    menu->addAction(tr("Merge from here"));
    menu->addAction(tr("Merge into here"));
    menu->addSeparator();
    menu->addAction(QIcon::fromTheme("edit-rename"), tr("Rename"));
    menu->addAction(tr("Tag"));
    menu->addAction(tr("Branch from here"));
    menu->addSeparator();
    menu->addAction(QIcon::fromTheme("edit-delete"), tr("Delete"));
    connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
    menu->popup(event->globalPos());
}
