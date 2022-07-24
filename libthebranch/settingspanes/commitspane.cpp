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

#include <git2.h>

struct CommitsPanePrivate {
        git_config* config;
};

CommitsPane::CommitsPane(QWidget* parent) :
    tSettingsPane(parent),
    ui(new Ui::CommitsPane) {
    ui->setupUi(this);
    d = new CommitsPanePrivate();

    git_config_open_default(&d->config);
    git_config* snapshot;
    git_config_snapshot(&snapshot, d->config);

    const char *name, *email;
    if (git_config_get_string(&name, snapshot, "user.name") == 0) {
        ui->nameEdit->setText(QString::fromUtf8(name));
    }

    if (git_config_get_string(&email, snapshot, "user.email") == 0) {
        ui->emailEdit->setText(QString::fromUtf8(email));
    }
    git_config_free(snapshot);
}

CommitsPane::~CommitsPane() {
    git_config_free(d->config);
    delete d;
    delete ui;
}

QString CommitsPane::paneName() {
    return tr("Commits");
}

void CommitsPane::on_nameEdit_textChanged(const QString& arg1) {
    git_config_set_string(d->config, "user.name", arg1.toUtf8().data());
}

void CommitsPane::on_emailEdit_textChanged(const QString& arg1) {
    git_config_set_string(d->config, "user.email", arg1.toUtf8().data());
}
