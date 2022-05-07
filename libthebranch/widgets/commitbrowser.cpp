#include "commitbrowser.h"

#include "objects/commitmodel.h"
#include "objects/repository.h"

#include <QContextMenuEvent>
#include <QMenu>
#include <tmessagebox.h>

struct CommitBrowserPrivate {
        RepositoryPtr repo = nullptr;
        CommitModel* model;
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
}

void CommitBrowser::contextMenuEvent(QContextMenuEvent* event) {
    if (this->selectedIndexes().isEmpty()) return;

    QModelIndex index = this->selectedIndexes().first();

    QMenu* menu = new QMenu();
    menu->addSection(tr("For commit %1").arg(QLocale().quoteString(index.data(CommitModel::CommitHash).toString())));
    menu->addAction(tr("Checkout"), this, [=] {
        //        CommitPtr commit = index.data(CommitModel::Commit).value<CommitPtr>();
        //        d->repo->setHeadAndCheckout(commit);
    });
    menu->addSeparator();
    menu->addAction(tr("Copy Commit Message"));
    menu->addAction(tr("Copy Commit Hash"));
    menu->addSeparator();
    menu->addAction(tr("Tag"));
    menu->addAction(tr("Cherry Pick"), this, [=] {
        //        tMessageBox* box = new tMessageBox(this->window());
        //        box->setTitleBarText(tr("Selektiv udvælgelse?"));
        //        box->setMessageText(tr("Vil du bruge %1 til %2?").arg(QLocale().quoteString(index.data(CommitModel::CommitHash).toString()), QLocale().quoteString("main")));
        //        box->addButton("Selektiv udvælgelse", QMessageBox::AcceptRole);
        //        box->addButton(tr("Afbestille"), QMessageBox::RejectRole);
        //        //        box->setIcon(QMessageBox::Critical);
        //        box->exec(true);
    });
    menu->addAction(tr("Branch from here"));
    menu->addAction(tr("Create Revert Commit"));
    connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
    menu->popup(event->globalPos());
}
