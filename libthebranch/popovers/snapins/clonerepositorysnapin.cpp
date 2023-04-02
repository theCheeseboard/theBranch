#include "clonerepositorysnapin.h"
#include "ui_clonerepositorysnapin.h"

#include "../snapinpopover.h"
#include "objects/libgit/lgactiveremote.h"
#include "objects/repository.h"
#include <QFileDialog>
#include <tcontentsizer.h>
#include <terrorflash.h>

struct CloneRepositorySnapInPrivate {
        QList<QVariantMap> creds;
};

CloneRepositorySnapIn::CloneRepositorySnapIn(QWidget* parent) :
    SnapIn(parent),
    ui(new Ui::CloneRepositorySnapIn) {
    ui->setupUi(this);

    d = new CloneRepositorySnapInPrivate();

    ui->titleLabel->setBackButtonShown(true);

    new tContentSizer(ui->cloneOptionsWidget);
    new tContentSizer(ui->cloneOptionsWidget_2);

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

    auto originalInformationRequiredCallback = this->parentPopover()->getInformationRequiredCallback();
    auto informationRequiredCallback = [this, originalInformationRequiredCallback](QVariantMap params) -> QCoro::Task<QVariantMap> {
        auto response = co_await originalInformationRequiredCallback(params);
        if (response.isEmpty()) throw QException();

        d->creds.append(response);

        co_return response;
    };

    ui->stackedWidget->setCurrentWidget(ui->cloningPage);
    auto remote = LGActiveRemote::fromDetached(ui->cloneUrlEdit->text());
    remote->setInformationRequiredCallback(informationRequiredCallback);

    try {
        co_await remote->connect(false);
        auto refs = co_await remote->fetchRefspecs();

        ui->branchComboBox->clear();
        QStringList branches;
        for (auto ref : co_await remote->ls()) {
            if (ref.startsWith("refs/heads/")) branches.append(ref);
        }

        for (auto branch : branches) {
            ui->branchComboBox->addItem(branch.mid(11));
        }
        auto defaultBranch = co_await remote->defaultBranch();
        ui->branchComboBox->setCurrentIndex(branches.indexOf(defaultBranch));

        if (ui->branchComboBox->count() == 0) {
            ui->branchStatus->setState(tStatusFrame::Warning);
            ui->branchStatus->setTitle(tr("Heads up!").toUpper());
            ui->branchStatus->setText(tr("You appear to be cloning an empty repository"));
            ui->branchStatus->setVisible(true);
            ui->branchComboBox->setEnabled(false);
        } else {
            ui->branchStatus->setVisible(false);
            ui->branchComboBox->setEnabled(true);
        }

        ui->stackedWidget->setCurrentWidget(ui->branchPage);
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

void CloneRepositorySnapIn::on_titleLabel_2_backButtonClicked() {
    ui->stackedWidget->setCurrentWidget(ui->clonePage);
}

void CloneRepositorySnapIn::on_cloneButton_2_clicked() {
    // Sanity checks
    if (ui->cloneDirectoryEdit->text().isEmpty()) {
        tErrorFlash::flashError(ui->cloneDirectoryEdit, tr("Clone Directory cannot be blank"));
        return;
    }

    QDir dir(ui->cloneDirectoryEdit->text());
    if (!dir.exists()) {
        tErrorFlash::flashError(ui->cloneDirectoryEdit, tr("Clone Directory does not exist"));
        return;
    }

    if (!dir.entryList(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs | QDir::Hidden | QDir::System).isEmpty()) {
        tErrorFlash::flashError(ui->cloneDirectoryEdit, tr("Clone Directory is not empty"));
        return;
    }

    auto creds = d->creds;
    auto retrieveInformationRequiredCallback = [this, creds](QVariantMap params) -> QCoro::Task<QVariantMap> {
        auto c = creds.at(0);
        //            credIdx++;
        co_return c;
    };

    QVariantMap options;
    if (ui->branchComboBox->isEnabled()) options.insert("branch", ui->branchComboBox->currentText());

    RepositoryPtr repo = Repository::cloneRepository(ui->cloneUrlEdit->text(), ui->cloneDirectoryEdit->text(), retrieveInformationRequiredCallback, options);
    emit openRepository(repo);
    emit done();
}
