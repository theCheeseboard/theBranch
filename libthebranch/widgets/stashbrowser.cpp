#include "stashbrowser.h"

#include "objects/branchuihelper.h"
#include "objects/stash.h"
#include "objects/stashesmodel.h"
#include <QContextMenuEvent>
#include <QMenu>

struct StashBrowserPrivate {
        RepositoryPtr repo;
        StashesModel* model;
};

StashBrowser::StashBrowser(QWidget* parent) :
    QListView{parent} {
    d = new StashBrowserPrivate();

    d->model = new StashesModel();
    this->setModel(d->model);
    this->setFrameShape(QFrame::NoFrame);
}

StashBrowser::~StashBrowser() {
    delete d;
}

void StashBrowser::setRepository(RepositoryPtr repo) {
    d->repo = repo;
    d->model->setRepository(repo);
}

void StashBrowser::contextMenuEvent(QContextMenuEvent* event) {
    if (this->selectedIndexes().isEmpty()) return;
    QModelIndex index = this->selectedIndexes().first();

    auto* menu = new QMenu();
    BranchUiHelper::appendStashMenu(menu, index.data(StashesModel::Stash).value<StashPtr>(), d->repo, this);
    connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
    menu->popup(event->globalPos());
}
