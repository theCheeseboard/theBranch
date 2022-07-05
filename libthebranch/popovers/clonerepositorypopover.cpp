#include "clonerepositorypopover.h"
#include "ui_clonerepositorypopover.h"

#include "objects/repository.h"
#include <QFileDialog>
#include <tcontentsizer.h>
#include <terrorflash.h>

CloneRepositoryPopover::CloneRepositoryPopover(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::CloneRepositoryPopover) {
    ui->setupUi(this);

    ui->titleLabel->setBackButtonShown(true);
    ui->titleLabel_2->setBackButtonShown(true);

    new tContentSizer(ui->cloneOptionsWidget);
    new tContentSizer(ui->burnConfirmWidget);

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
}

CloneRepositoryPopover::~CloneRepositoryPopover() {
    delete ui;
}

void CloneRepositoryPopover::on_titleLabel_backButtonClicked() {
    emit done();
}

void CloneRepositoryPopover::on_cloneButton_clicked() {
    // Sanity checks
    if (ui->cloneUrlEdit->text().isEmpty()) {
        tErrorFlash::flashError(ui->cloneUrlEdit, tr("Clone URL cannot be blank"));
        return;
    }

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

    RepositoryPtr repo = Repository::cloneRepository(ui->cloneUrlEdit->text(), ui->cloneDirectoryEdit->text(), {});
    emit openRepository(repo);
    emit done();
}

void CloneRepositoryPopover::on_browseButton_clicked() {
    QFileDialog* dialog = new QFileDialog(this);
    dialog->setAcceptMode(QFileDialog::AcceptOpen);
    dialog->setFileMode(QFileDialog::Directory);
    connect(dialog, &QFileDialog::fileSelected, this, [this](QString file) {
        ui->cloneDirectoryEdit->setText(file);
    });
    connect(dialog, &QFileDialog::finished, dialog, &QFileDialog::deleteLater);
    dialog->open();
}
