#include "usernamepasswordsnapin.h"
#include "ui_usernamepasswordsnapin.h"

#include <tcontentsizer.h>

struct UsernamePasswordSnapInPrivate {
};

UsernamePasswordSnapIn::UsernamePasswordSnapIn(QVariantMap params, QWidget* parent) :
    InformationRequestSnapIn(parent),
    ui(new Ui::UsernamePasswordSnapIn) {
    ui->setupUi(this);
    d = new UsernamePasswordSnapInPrivate();
    ui->titleLabel->setBackButtonShown(true);
    new tContentSizer(ui->usernamePasswordOptionsWidget);
    new tContentSizer(ui->authenticateButton);

    ui->usernamePasswordAuthenticationPrompt->setText(tr("Enter a username and password to authenticate to %1").arg(QLocale().quoteString(params.value("url").toString())));
}

UsernamePasswordSnapIn::~UsernamePasswordSnapIn() {
    delete d;
    delete ui;
}

void UsernamePasswordSnapIn::on_titleLabel_backButtonClicked() {
    emit response({});
    emit done();
}

void UsernamePasswordSnapIn::on_authenticateButton_clicked() {
    QVariantMap response;
    response.insert("username", ui->usernameEdit->text());
    response.insert("password", ui->passwordEdit->text());

    emit this->response(response);
    emit done();
}
