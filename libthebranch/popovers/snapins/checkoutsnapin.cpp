#include "checkoutsnapin.h"
#include "ui_checkoutsnapin.h"

#include <tcontentsizer.h>

struct CheckoutSnapInPrivate {
        RepositoryPtr repository;
};

CheckoutSnapIn::CheckoutSnapIn(RepositoryPtr repository, QWidget* parent) :
    SnapIn(parent),
    ui(new Ui::CheckoutSnapIn) {
    ui->setupUi(this);
    d = new CheckoutSnapInPrivate();
    d->repository = repository;

    ui->titleLabel->setBackButtonShown(true);
    new tContentSizer(ui->checkoutOptionsWidget);
    new tContentSizer(ui->checkoutButton);
}

CheckoutSnapIn::~CheckoutSnapIn() {
    delete ui;
    delete d;
}

void CheckoutSnapIn::on_titleLabel_backButtonClicked() {
    emit done();
}

void CheckoutSnapIn::on_checkoutButton_clicked() {
}
