#include "stashsavesnapin.h"
#include "ui_stashsavesnapin.h"

#include "objects/repository.h"
#include <tcontentsizer.h>

struct StashSaveSnapInPrivate {
        RepositoryPtr repo;
};

StashSaveSnapIn::StashSaveSnapIn(RepositoryPtr repo, QWidget* parent) :
    SnapIn(parent),
    ui(new Ui::StashSaveSnapIn) {
    ui->setupUi(this);
    d = new StashSaveSnapInPrivate();
    d->repo = repo;

    ui->titleLabel->setBackButtonShown(true);
    new tContentSizer(ui->stashOptionsWidget);
    new tContentSizer(ui->stashButton);
    new tContentSizer(ui->errorFrame);

    ui->errorFrame->setState(tStatusFrame::Error);
    ui->errorFrame->setTitle(tr("Can't stash your changes"));
    ui->errorFrame->setVisible(false);

    ui->stashMessageBox->setPlaceholderText(tr("Stash saved at %1").arg(QLocale().toString(QDateTime::currentDateTime())));
}

StashSaveSnapIn::~StashSaveSnapIn() {
    delete d;
    delete ui;
}

void StashSaveSnapIn::on_titleLabel_backButtonClicked() {
    emit done();
}

QCoro::Task<> StashSaveSnapIn::on_stashButton_clicked() {
    QString message = ui->stashMessageBox->text();
    if (message.isEmpty()) message = ui->stashMessageBox->placeholderText();

    try {
        co_await d->repo->stash(message);
        emit done();
    } catch (GitException& ex) {
        ui->errorFrame->setText(ex.description());
        ui->errorFrame->setVisible(true);
    }
}
