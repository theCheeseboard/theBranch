#include "commitbrowser.h"

#include "objects/branchuihelper.h"
#include "objects/commit.h"
#include "objects/commitmodel.h"
#include "objects/repository.h"

#include <QContextMenuEvent>
#include <QMenu>
#include <tmessagebox.h>

struct CommitBrowserPrivate {
        RepositoryPtr repo = nullptr;
        CommitModel* model;

        BranchPtr startPoint;
};

CommitBrowser::CommitBrowser(QWidget* parent) :
    QListView{parent} {
    d = new CommitBrowserPrivate();

    d->model = new CommitModel();
    this->setModel(d->model);
    this->setItemDelegate(new CommitDelegate(this));
    this->setFrameShape(QFrame::NoFrame);
}

CommitBrowser::~CommitBrowser() {
    delete d;
}

void CommitBrowser::setRepository(RepositoryPtr repo) {
    d->repo = repo;
    d->model->setRepository(repo);
    connect(repo.data(), &Repository::repositoryUpdated, this, &CommitBrowser::reloadData);
    this->reloadData();
}

void CommitBrowser::setStartBranch(BranchPtr branch) {
    d->startPoint = branch;
    this->reloadData();
}

void CommitBrowser::reloadData() {
    d->model->setStartPoint(d->startPoint ? d->startPoint->lastCommit() : nullptr);
}

void CommitBrowser::contextMenuEvent(QContextMenuEvent* event) {
    if (this->selectedIndexes().isEmpty()) return;

    QModelIndex index = this->selectedIndexes().first();

    QMenu* menu = new QMenu();
    BranchUiHelper::appendCommitMenu(menu, index.data(CommitModel::Commit).value<CommitPtr>(), d->repo, this);
    connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
    menu->popup(event->globalPos());
}
