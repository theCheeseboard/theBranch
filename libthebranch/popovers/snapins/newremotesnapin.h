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
#ifndef NEWREMOTEPOPOVER_H
#define NEWREMOTEPOPOVER_H

#include "libthebranch_global.h"
#include "objects/forward_declares.h"
#include "snapin.h"
#include <QCoroTask>
#include <QWidget>

namespace Ui {
    class NewRemoteSnapIn;
}

struct NewRemoteSnapInPrivate;
class LIBTHEBRANCH_EXPORT NewRemoteSnapIn : public SnapIn {
        Q_OBJECT

    public:
        explicit NewRemoteSnapIn(RepositoryPtr repo, QWidget* parent = nullptr);
        ~NewRemoteSnapIn();

    private slots:
        void on_titleLabel_backButtonClicked();

        QCoro::Task<> on_addRemoteButton_clicked();

    private:
        Ui::NewRemoteSnapIn* ui;
        NewRemoteSnapInPrivate* d;
};

#endif // NEWREMOTEPOPOVER_H
