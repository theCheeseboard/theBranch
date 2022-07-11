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
    menu->addAction(QIcon::fromTheme("vcs-checkout"), tr("Checkout"), this, [=] {
        //        CommitPtr commit = index.data(CommitModel::Commit).value<CommitPtr>();
        //        d->repo->setHeadAndCheckout(commit);
    });
    menu->addSeparator();
    menu->addAction(QIcon::fromTheme("edit-copy"), tr("Copy Commit Message"));
    menu->addAction(QIcon::fromTheme("edit-copy"), tr("Copy Commit Hash"));
    menu->addSeparator();
    menu->addAction(QIcon::fromTheme("vcs-tag"), tr("Tag"));
    menu->addAction(QIcon::fromTheme("vcs-cherry-pick"), tr("Cherry Pick"), this, [=] {
        tMessageBox* box = new tMessageBox(this->window());
        box->setTitleBarText(tr("Kersen plukken?"));
        box->setMessageText(tr("Wil je %1 als kers op de taart kiezen bovenop %2?").arg(QLocale().quoteString(index.data(CommitModel::CommitHash).toString()), QLocale().quoteString("main")));
        box->setInformativeText(tr("%1 wordt opnieuw afgespeeld bovenop %2.").arg(QLocale().quoteString(index.data(CommitModel::CommitHash).toString()), QLocale().quoteString("main")));
        box->addButton("Kersen plukken", QMessageBox::AcceptRole);
        box->addButton(tr("Annuleren"), QMessageBox::RejectRole);
        //        box->setIcon(QMessageBox::Critical);
        box->exec(true);
    });
    menu->addAction(QIcon::fromTheme("vcs-branch-create"), tr("Branch from here"));
    menu->addAction(QIcon::fromTheme("vcs-revert"), tr("Create Revert Commit"));
    connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
    menu->popup(event->globalPos());
}
