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
#ifndef ACCOUNTSPANE_H
#define ACCOUNTSPANE_H

#include "libthebranch_global.h"
#include <tsettingswindow/tsettingspane.h>

namespace Ui {
    class AccountsPane;
}

struct AccountsPanePrivate;
class LIBTHEBRANCH_EXPORT AccountsPane : public tSettingsPane {
        Q_OBJECT

    public:
        explicit AccountsPane(QWidget* parent = nullptr);
        ~AccountsPane();

    private:
        Ui::AccountsPane* ui;
        AccountsPanePrivate* d;

        void loadAccounts();

        // tSettingsPane interface
    public:
        QString paneName();
    private slots:
        void on_commandLinkButton_clicked();
        void on_accountsList_customContextMenuRequested(const QPoint& pos);
};

#endif // ACCOUNTSPANE_H
