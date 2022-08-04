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
#include "accountspane.h"
#include "ui_accountspane.h"

#include "accounts/abstractaccount.h"
#include "accounts/accountsmanager.h"
#include "accounts/github/addgithubaccountpopover.h"
#include "branchservices.h"
#include <QMenu>
#include <tpopover.h>

struct AccountsPanePrivate {
};

AccountsPane::AccountsPane(QWidget* parent) :
    tSettingsPane(parent),
    ui(new Ui::AccountsPane) {
    ui->setupUi(this);
    d = new AccountsPanePrivate();

    connect(BranchServices::accounts(), &AccountsManager::accountsChanged, this, &AccountsPane::loadAccounts);
    loadAccounts();
}

AccountsPane::~AccountsPane() {
    delete d;
    delete ui;
}

void AccountsPane::loadAccounts() {
    ui->accountsList->clear();
    for (auto* account : BranchServices::accounts()->accounts()) {
        auto* item = new QListWidgetItem();
        item->setText(account->description());
        item->setData(Qt::UserRole, QVariant::fromValue(account));
        ui->accountsList->addItem(item);
    }
}

QString AccountsPane::paneName() {
    return tr("Accounts");
}

void AccountsPane::on_commandLinkButton_clicked() {
    auto* jp = new AddGithubAccountPopover();
    auto* popover = new tPopover(jp);
    popover->setPopoverWidth(SC_DPI_W(600, this));
    popover->setPopoverSide(tPopover::Trailing);
    connect(jp, &AddGithubAccountPopover::done, popover, &tPopover::dismiss);
    connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
    connect(popover, &tPopover::dismissed, jp, &AddGithubAccountPopover::deleteLater);
    popover->show(this->window());
}

void AccountsPane::on_accountsList_customContextMenuRequested(const QPoint& pos) {
    auto selected = ui->accountsList->selectedItems();
    if (selected.isEmpty()) return;

    auto* menu = new QMenu();
    menu->addAction(QIcon::fromTheme("list-remove"), tr("Remove Account"), this, [this, selected] {
        auto* account = selected.first()->data(Qt::UserRole).value<AbstractAccount*>();
        BranchServices::accounts()->removeAccount(account);
    });
    menu->popup(ui->accountsList->mapToGlobal(pos));
}
