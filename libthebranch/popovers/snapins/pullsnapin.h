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
#ifndef PULLSNAPIN_H
#define PULLSNAPIN_H

#include "libthebranch_global.h"
#include "objects/forward_declares.h"
#include "snapin.h"
#include <QCoroTask>

namespace Ui {
    class PullSnapIn;
}

struct PullSnapInPrivate;
class LIBTHEBRANCH_EXPORT PullSnapIn : public SnapIn {
        Q_OBJECT

    public:
        explicit PullSnapIn(RepositoryPtr repo, QWidget* parent = nullptr);
        ~PullSnapIn();

    private slots:
        void on_titleLabel_backButtonClicked();

        QCoro::Task<> on_pullButton_clicked();

    private:
        Ui::PullSnapIn* ui;
        PullSnapInPrivate* d;
};

#endif // PULLSNAPIN_H
