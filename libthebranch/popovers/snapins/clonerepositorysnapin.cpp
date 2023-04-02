#include "clonerepositorysnapin.h"
#include "ui_clonerepositorysnapin.h"

#include "../snapinpopover.h"
#include "objects/libgit/lgactiveremote.h"
#include "objects/repository.h"
#include <QFileDialog>
#include <tcontentsizer.h>
#include <terrorflash.h>

struct CloneRepositorySnapInPrivate {
};

CloneRepositorySnapIn::CloneRepositorySnapIn(QWidget* parent) :
    SnapIn(parent),
    ui(new Ui::CloneRepositorySnapIn) {
    ui->setupUi(this);

    d = new CloneRepositorySnapInPrivate();

    ui->titleLabel->setBackButtonShown(true);

    new tContentSizer(ui->cloneOptionsWidget);

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
}

CloneRepositorySnapIn::~CloneRepositorySnapIn() {
    delete d;
    delete ui;
}

void CloneRepositorySnapIn::on_titleLabel_backButtonClicked() {
    emit done();
}

QCoro::Task<> CloneRepositorySnapIn::on_cloneButton_clicked() {
    // Sanity checks
    if (ui->cloneUrlEdit->text().isEmpty()) {
        tErrorFlash::flashError(ui->cloneUrlEdit, tr("Clone URL cannot be blank"));
        co_return;
    }

    if (ui->cloneDirectoryEdit->text().isEmpty()) {
        tErrorFlash::flashError(ui->cloneDirectoryEdit, tr("Clone Directory cannot be blank"));
        co_return;
    }

    QDir dir(ui->cloneDirectoryEdit->text());
    if (!dir.exists()) {
        tErrorFlash::flashError(ui->cloneDirectoryEdit, tr("Clone Directory does not exist"));
        co_return;
    }

    if (!dir.entryList(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs | QDir::Hidden | QDir::System).isEmpty()) {
        tErrorFlash::flashError(ui->cloneDirectoryEdit, tr("Clone Directory is not empty"));
        co_return;
    }

    QList<QVariantMap> creds;
    //    int credIdx = 0;

    auto originalInformationRequiredCallback = this->parentPopover()->getInformationRequiredCallback();
    auto informationRequiredCallback = [this, originalInformationRequiredCallback, &creds](QVariantMap params) -> QCoro::Task<QVariantMap> {
        auto response = co_await originalInformationRequiredCallback(params);
        if (response.isEmpty()) throw QException();

        creds.append(response);

        co_return response;
    };

    ui->stackedWidget->setCurrentWidget(ui->cloningPage);
    auto remote = LGActiveRemote::fromDetached(ui->cloneUrlEdit->text());
    remote->setInformationRequiredCallback(informationRequiredCallback);

    try {
        co_await remote->connect(false);
        auto refs = co_await remote->fetchRefspecs();

        auto retrieveInformationRequiredCallback = [this, creds](QVariantMap params) -> QCoro::Task<QVariantMap> {
            auto c = creds.at(0);
            //            credIdx++;
            co_return c;
        };

        RepositoryPtr repo = Repository::cloneRepository(ui->cloneUrlEdit->text(), ui->cloneDirectoryEdit->text(), retrieveInformationRequiredCallback, {});
        emit openRepository(repo);
        emit done();
    } catch (GitException ex) {
        tErrorFlash::flashError(ui->cloneUrlEdit, ex.description());
        ui->stackedWidget->setCurrentWidget(ui->clonePage);
    }
}

void CloneRepositorySnapIn::on_browseButton_clicked() {
    QFileDialog* dialog = new QFileDialog(this);
    dialog->setAcceptMode(QFileDialog::AcceptOpen);
    dialog->setFileMode(QFileDialog::Directory);
    connect(dialog, &QFileDialog::fileSelected, this, [this](QString file) {
        ui->cloneDirectoryEdit->setText(file);
    });
    connect(dialog, &QFileDialog::finished, dialog, &QFileDialog::deleteLater);
    dialog->open();
}
