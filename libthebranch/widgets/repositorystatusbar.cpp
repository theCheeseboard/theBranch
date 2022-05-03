#include "repositorystatusbar.h"
#include "ui_repositorystatusbar.h"

#include "objects/reference.h"
#include "objects/repository.h"

struct RepositoryStatusBarPrivate {
        RepositoryPtr repository;
};

RepositoryStatusBar::RepositoryStatusBar(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::RepositoryStatusBar) {
    ui->setupUi(this);
    d = new RepositoryStatusBarPrivate();
}

RepositoryStatusBar::~RepositoryStatusBar() {
    delete d;
    delete ui;
}

void RepositoryStatusBar::setRepository(RepositoryPtr repository) {
    if (d->repository) d->repository->disconnect(this);
    d->repository = repository;
    if (d->repository) {
        connect(d->repository.data(), &Repository::repositoryUpdated, this, &RepositoryStatusBar::updateRepository);
        updateRepository();
    }
}

void RepositoryStatusBar::updateRepository() {
    ReferencePtr ref = d->repository->head();
    if (ref) {
        ui->headLabel->setText(ref->shorthand());
    } else {
        ui->headLabel->setText(tr("(no HEAD)"));
    }
}
