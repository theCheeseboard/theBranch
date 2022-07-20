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
#ifndef REBASESNAPIN_H
#define REBASESNAPIN_H

#include "libthebranch_global.h"
#include "objects/forward_declares.h"
#include "snapin.h"

namespace Ui {
    class RebaseSnapIn;
}

struct RebaseSnapInPrivate;
class LIBTHEBRANCH_EXPORT RebaseSnapIn : public SnapIn {
        Q_OBJECT

    public:
        explicit RebaseSnapIn(RebasePtr rebase, QWidget* parent = nullptr);
        ~RebaseSnapIn();

    private slots:
        void on_titleLabel_backButtonClicked();

        void on_rebaseButton_clicked();

    private:
        Ui::RebaseSnapIn* ui;
        RebaseSnapInPrivate* d;
};

#endif // REBASESNAPIN_H
