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
#include "newremotesnapin.h"
#include "ui_newremotesnapin.h"

#include "objects/errorresponse.h"
#include "objects/remote.h"
#include "objects/repository.h"
#include "popovers/snapinpopover.h"
#include <tcontentsizer.h>
#include <terrorflash.h>

struct NewRemoteSnapInPrivate {
        RepositoryPtr repo;
};

NewRemoteSnapIn::NewRemoteSnapIn(RepositoryPtr repo, QWidget* parent) :
    SnapIn(parent),
    ui(new Ui::NewRemoteSnapIn) {
    ui->setupUi(this);
    d = new NewRemoteSnapInPrivate();
    d->repo = repo;

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
    ui->titleLabel->setBackButtonShown(true);
    new tContentSizer(ui->remoteOptionsWidget);
    new tContentSizer(ui->addRemoteButton);
    new tContentSizer(ui->remoteErrorFrame);
    ui->spinner->setFixedSize(SC_DPI_WT(QSize(32, 32), QSize, this));

    ui->remoteErrorFrame->setState(tStatusFrame::Error);
    ui->remoteErrorFrame->setVisible(false);
    ui->remoteErrorFrame->setTitle(tr("Could not connect to the remote"));
}

NewRemoteSnapIn::~NewRemoteSnapIn() {
    delete d;
    delete ui;
}

void NewRemoteSnapIn::on_titleLabel_backButtonClicked() {
    emit done();
}

QCoro::Task<> NewRemoteSnapIn::on_addRemoteButton_clicked() {
    if (ui->nameBox->text().isEmpty()) {
        tErrorFlash::flashError(ui->nameContainer);
        co_return;
    }

    if (ui->urlBox->text().isEmpty()) {
        tErrorFlash::flashError(ui->urlContainer);
        co_return;
    }

    auto remote = d->repo->addRemote(ui->nameBox->text(), ui->urlBox->text());
    if (!remote) {
        auto error = ErrorResponse::fromCurrentGitError();
        tErrorFlash::flashError(ui->nameContainer, error.description());
        co_return;
    }

    ui->stackedWidget->setCurrentWidget(ui->addingPage);
    try {
        QStringList refs;
        //        refs.append(QStringLiteral("+refs/heads/*:refs/remotes/%1/*").arg(remote->name()));
        co_await remote->fetch(refs, parentPopover()->getInformationRequiredCallback());
        emit done();
    } catch (GitException& ex) {
        remote->remove();

        QTimer::singleShot(500, this, [this, ex] {
            ui->remoteErrorFrame->setVisible(true);
            ui->remoteErrorFrame->setText(ex.description());
            ui->stackedWidget->setCurrentWidget(ui->remoteOptionsPage);
        });
    } catch (QException& ex) {
        remote->remove();

        QTimer::singleShot(500, this, [this] {
            ui->remoteErrorFrame->setVisible(true);
            ui->remoteErrorFrame->setText(tr("Ensure the URL is correct and that your Internet connection is working"));
            ui->stackedWidget->setCurrentWidget(ui->remoteOptionsPage);
        });
    }
}
