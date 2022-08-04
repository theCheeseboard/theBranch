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
#include "sshcertchecksnapin.h"
#include "ui_sshcertchecksnapin.h"

#include <tcontentsizer.h>

SshCertCheckSnapIn::SshCertCheckSnapIn(QVariantMap params, QWidget* parent) :
    InformationRequestSnapIn(parent),
    ui(new Ui::SshCertCheckSnapIn) {
    ui->setupUi(this);

    ui->titleLabel->setBackButtonShown(true);
    new tContentSizer(ui->sshKeyOptionsWidget);
    new tContentSizer(ui->warningFrame);
    new tContentSizer(ui->acceptButton);

    ui->hostLabel->setText(params.value("host").toString());
    ui->fingerprintLabel->setText(params.value("hash-sha256").toString());

    if (params.value("unexpected").toBool()) {
        ui->warningFrame->setState(tStatusFrame::Error);
        ui->warningFrame->setTitle(tr("Remote Host Identification has changed"));
        ui->warningFrame->setText(tr("IT IS POSSIBLE THAT SOMEONE IS DOING SOMETHING NASTY.\nSomeone could be eavesdropping on you right now!\n\nIt is also possible that a host key has just been changed."));

        auto continueButton = ui->warningFrame->addButton();
        continueButton->setFlat(true);
        continueButton->setText(tr("Continue Nevertheless"));
        connect(continueButton, &QPushButton::clicked, this, [this] {
            ui->warningFrame->setVisible(false);
            ui->acceptButton->setEnabled(true);
        });

        auto stopButton = ui->warningFrame->addButton();
        stopButton->setText(tr("Abort Operation"));
        connect(stopButton, &QPushButton::clicked, this, [this] {
            emit response({});
            emit done();
        });

        ui->acceptButton->setEnabled(false);
    } else {
        ui->warningFrame->setVisible(false);
    }
}

SshCertCheckSnapIn::~SshCertCheckSnapIn() {
    delete ui;
}

void SshCertCheckSnapIn::on_titleLabel_backButtonClicked() {
    emit response({});
    emit done();
}

void SshCertCheckSnapIn::on_acceptButton_clicked() {
    emit response({
        {"ok", true}
    });
    emit done();
}
