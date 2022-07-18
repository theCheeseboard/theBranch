#include "branchbrowser.h"

#include "objects/branchmodel.h"
#include "objects/branchuihelper.h"
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
    BranchUiHelper::appendBranchMenu(menu, index.data(BranchModel::Branch).value<BranchPtr>(), d->repo, this);
    //    connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
    menu->popup(event->globalPos());
}
