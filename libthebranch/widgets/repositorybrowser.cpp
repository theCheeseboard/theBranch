#include "repositorybrowser.h"
#include "ui_repositorybrowser.h"

#include "commandpalettes/branchescommandpalette.h"
#include "objects/repository.h"
// #include "popovers/clonerepositorysnapin.h"
#include <QFileDialog>
#include <git2.h>
#include <tmessagebox.h>
#include <tpopover.h>

struct RepositoryBrowserPrivate {
        RepositoryPtr repository;
};

RepositoryBrowser::RepositoryBrowser(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::RepositoryBrowser) {
    ui->setupUi(this);
    d = new RepositoryBrowserPrivate();

    ui->stackedWidget->setCurrentWidget(ui->initialPage, false);
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::Fade);
    ui->stackedWidget_2->setCurrentAnimation(tStackedWidget::Lift);
    ui->leftWidget->setFixedWidth(SC_DPI_W(300, this));

    connect(ui->leftView, &RepositoryBrowserList::showWidget, this, [this](QWidget* widget) {
        ui->stackedWidget_2->addWidget(widget);
        ui->stackedWidget_2->setCurrentWidget(widget);
    });
}

RepositoryBrowser::~RepositoryBrowser() {
    delete ui;
    delete d;
}

void RepositoryBrowser::setRepository(RepositoryPtr repository) {
    if (d->repository) d->repository->disconnect(this);
    d->repository = repository;

    ui->leftView->setRepository(repository);
    ui->repositoryStatus->setRepository(repository);

    connect(d->repository.data(), &Repository::stateChanged, this, &RepositoryBrowser::updateRepositoryState);
    connect(d->repository.data(), &Repository::stateDescriptionChanged, this, [=] {
        ui->stateDescription->setText(d->repository->stateDescription());
    });
    connect(d->repository.data(), &Repository::stateInformationalTextChanged, this, [=] {
        ui->stateInformationalText->setText(d->repository->stateInformationalText());
    });
    connect(d->repository.data(), &Repository::stateProgressChanged, this, [=] {
        ui->stateProgress->setVisible(d->repository->stateProvidesProgress());
        ui->stateProgress->setValue(d->repository->stateProgress());
        ui->stateProgress->setMaximum(d->repository->stateTotalProgress());
    });

    this->updateRepositoryState();
    ui->stateDescription->setText(d->repository->stateDescription());
    ui->stateInformationalText->setText(d->repository->stateInformationalText());
    ui->stateProgress->setVisible(d->repository->stateProvidesProgress());
    ui->stateProgress->setValue(d->repository->stateProgress());
    ui->stateProgress->setMaximum(d->repository->stateTotalProgress());

    emit repositoryChanged();
}

RepositoryPtr RepositoryBrowser::repository() {
    return d->repository;
}

QString RepositoryBrowser::title() {
    if (!d->repository) return tr("Welcome");

    switch (d->repository->state()) {
        case Repository::Invalid:
            return tr("Invalid Repository");
        case Repository::Cloning:
            return tr("Cloning...");
        case Repository::Idle:
            return QFileInfo(d->repository->repositoryPath()).fileName();
    }
}

void RepositoryBrowser::on_cloneButton_clicked() {
    //    CloneRepositorySnapIn* jp = new CloneRepositorySnapIn();
    //    tPopover* popover = new tPopover(jp);
    //    popover->setPopoverWidth(SC_DPI_W(-200, this));
    //    popover->setPopoverSide(tPopover::Bottom);
    //    connect(jp, &CloneRepositorySnapIn::done, popover, &tPopover::dismiss);
    //    connect(jp, &CloneRepositorySnapIn::openRepository, this, [=](RepositoryPtr repository) {
    //        this->setRepository(repository);
    //    });
    //    connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
    //    connect(popover, &tPopover::dismissed, jp, &CloneRepositorySnapIn::deleteLater);
    //    popover->show(this->window());
}

void RepositoryBrowser::updateRepositoryState() {
    if (d->repository->state() == Repository::Idle) {
        ui->stackedWidget->setCurrentWidget(ui->gitReadyPage);
    } else {
        ui->stackedWidget->setCurrentWidget(ui->gitStatePage);
    }

    emit titleChanged();
}

QCoro::Task<> RepositoryBrowser::on_openRepositoryButton_clicked() {
    try {
        auto repo = co_await Repository::repositoryForDirectoryUi(this);
        this->setRepository(repo);
    } catch (QException ex) {
        // Ignore
    }
}
