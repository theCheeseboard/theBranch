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

struct AccountsPanePrivate {
};

AccountsPane::AccountsPane(QWidget* parent) :
    tSettingsPane(parent),
    ui(new Ui::AccountsPane) {
    ui->setupUi(this);
    d = new AccountsPanePrivate();
}

AccountsPane::~AccountsPane() {
    delete d;
    delete ui;
}

QString AccountsPane::paneName() {
    return tr("Accounts");
}
