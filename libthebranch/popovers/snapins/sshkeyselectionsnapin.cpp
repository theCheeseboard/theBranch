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
#include "sshkeyselectionsnapin.h"
#include "ui_sshkeyselectionsnapin.h"

#include <QDir>
#include <tcontentsizer.h>

struct SshKeySelectionSnapInPrivate {
};

SshKeySelectionSnapIn::SshKeySelectionSnapIn(QVariantMap params, QWidget* parent) :
    InformationRequestSnapIn(parent),
    ui(new Ui::SshKeySelectionSnapIn) {
    ui->setupUi(this);
    d = new SshKeySelectionSnapInPrivate();

    ui->titleLabel->setBackButtonShown(true);
    new tContentSizer(ui->sshKeyOptionsWidget);
    new tContentSizer(ui->authenticateButton);

    ui->sshKeyAuthenticationPrompt->setText(tr("Select an SSH key to authenticate to %1").arg(QLocale().quoteString(params.value("url").toString())));

    // Search for SSH keys
    QDir sshDir = QDir::home().absoluteFilePath(".ssh");
    for (const auto& pk : sshDir.entryInfoList({"*.pub"})) {
        if (QFile::exists(sshDir.absoluteFilePath(pk.baseName()))) {
            ui->sshKeysBox->addItem(pk.baseName(), QStringList({pk.filePath(), sshDir.absoluteFilePath(pk.baseName())}));
        }
    }

    if (ui->sshKeysBox->count() == 0) ui->authenticateButton->setEnabled(false);
}

SshKeySelectionSnapIn::~SshKeySelectionSnapIn() {
    delete d;
    delete ui;
}

void SshKeySelectionSnapIn::on_titleLabel_backButtonClicked() {
    emit response({});
    emit done();
}

void SshKeySelectionSnapIn::on_authenticateButton_clicked() {
    auto keys = ui->sshKeysBox->currentData().toStringList();

    QVariantMap response;
    response.insert("pubKey", keys.at(0));
    response.insert("privKey", keys.at(1));

    emit this->response(response);
    emit done();
}
