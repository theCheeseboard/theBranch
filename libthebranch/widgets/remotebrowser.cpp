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
#include "remotebrowser.h"

#include "objects/remote.h"
#include "objects/remotesmodel.h"
#include "objects/repository.h"
#include "popovers/snapinpopover.h"
#include "popovers/snapins/newremotesnapin.h"
#include <QContextMenuEvent>
#include <QMenu>
#include <tmessagebox.h>
#include <tpopover.h>

struct RemoteBrowserPrivate {
        RepositoryPtr repo;
        RemotesModel* model;
};

RemoteBrowser::RemoteBrowser(QWidget* parent) :
    QListView{parent} {
    d = new RemoteBrowserPrivate();

    d->model = new RemotesModel();
    this->setModel(d->model);
    //    this->setItemDelegate(new CommitDelegate(this));
    this->setFrameShape(QFrame::NoFrame);
}

RemoteBrowser::~RemoteBrowser() {
    delete d;
}

void RemoteBrowser::setRepository(RepositoryPtr repo) {
    d->repo = repo;
    d->model->setRepository(repo);
}

void RemoteBrowser::contextMenuEvent(QContextMenuEvent* event) {
    QMenu* menu = new QMenu();

    if (!this->selectedIndexes().isEmpty()) {
        QModelIndex index = this->selectedIndexes().first();
        menu->addSection(tr("For remote %1").arg(QLocale().quoteString(index.data(Qt::DisplayRole).toString())));
        menu->addAction(QIcon::fromTheme("vcs-pull"), tr("Fetch"), this, [] {

        });
        menu->addAction(QIcon::fromTheme("list-remove"), tr("Delete Remote"), this, [this, index]() -> QCoro::Task<> {
            auto remote = index.data(RemotesModel::Remote).value<RemotePtr>();
            tMessageBox box(this->window());
            box.setTitleBarText(tr("Delete Remote"));
            box.setMessageText(tr("Do you want to delete %1?").arg(QLocale().quoteString(index.data(Qt::DisplayRole).toString())));
            box.setInformativeText(tr("Items on the remote will remain, but you won't be able to push or pull from it unless you add the remote again."));
            auto affirmativeButton = box.addButton(tr("Delete"), QMessageBox::DestructiveRole);
            box.addStandardButton(QMessageBox::Cancel);
            box.setIcon(QMessageBox::Question);
            auto button = co_await box.presentAsync();

            if (button == affirmativeButton) {
                remote->remove();
            }
        });
    }

    menu->addSection(tr("For repository"));
    menu->addAction(QIcon::fromTheme("list-add"), tr("Add Remote"), this, [this] {
        SnapInPopover::showSnapInPopover(this->window(), new NewRemoteSnapIn(d->repo));
    });

    connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
    menu->popup(event->globalPos());
}
