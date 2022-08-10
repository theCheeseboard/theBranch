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
#include "commitspane.h"
#include "ui_commitspane.h"

#include "objects/libgit/lgconfig.h"

struct CommitsPanePrivate {
    LGConfigPtr config;
};

CommitsPane::CommitsPane(QWidget* parent) :
    tSettingsPane(parent),
    ui(new Ui::CommitsPane) {
    ui->setupUi(this);
    d = new CommitsPanePrivate();
    d->config = LGConfig::defaultConfig();

    auto snapshot = d->config->snapshot();

    ui->nameEdit->setText(snapshot->getString("user.name"));
    ui->emailEdit->setText(snapshot->getString("user.email"));
}

CommitsPane::~CommitsPane() {
    delete d;
    delete ui;
}

QString CommitsPane::paneName() {
    return tr("Commits");
}

void CommitsPane::on_nameEdit_textChanged(const QString& arg1) {
    d->config->setString("user.name", arg1);
}

void CommitsPane::on_emailEdit_textChanged(const QString& arg1) {
    d->config->setString("user.email", arg1);
}
