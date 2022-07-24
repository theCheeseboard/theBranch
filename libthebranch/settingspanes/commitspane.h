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
#ifndef COMMITSPANE_H
#define COMMITSPANE_H

#include "libthebranch_global.h"
#include <tsettingswindow/tsettingspane.h>

namespace Ui {
    class CommitsPane;
}

struct CommitsPanePrivate;
class LIBTHEBRANCH_EXPORT CommitsPane : public tSettingsPane {
        Q_OBJECT

    public:
        explicit CommitsPane(QWidget* parent = nullptr);
        ~CommitsPane();

    private:
        Ui::CommitsPane* ui;
        CommitsPanePrivate* d;

        // tSettingsPane interface
    public:
        QString paneName();
    private slots:
        void on_nameEdit_textChanged(const QString& arg1);
        void on_emailEdit_textChanged(const QString& arg1);
};

#endif // COMMITSPANE_H
