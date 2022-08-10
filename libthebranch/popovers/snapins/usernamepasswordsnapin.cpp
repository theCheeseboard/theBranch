#include "usernamepasswordsnapin.h"
#include "ui_usernamepasswordsnapin.h"

#include "objects/libgit/lgrepository.h"
#include "objects/libgit/lgconfig.h"
#include <QProcess>
#include <QCoroProcess>
#include <tcontentsizer.h>

struct UsernamePasswordSnapInPrivate {
    LGRepositoryPtr repo;
    QString credentialHelper;
    QVariantMap params;
};

UsernamePasswordSnapIn::UsernamePasswordSnapIn(QVariantMap params, QWidget* parent) :
    InformationRequestSnapIn(parent),
    ui(new Ui::UsernamePasswordSnapIn) {
    ui->setupUi(this);
    d = new UsernamePasswordSnapInPrivate();
    d->params = params;

    ui->titleLabel->setBackButtonShown(true);
    new tContentSizer(ui->usernamePasswordOptionsWidget);
    new tContentSizer(ui->authenticateButton);
    new tContentSizer(ui->useCredentialHelperButton);

    ui->usernamePasswordAuthenticationPrompt->setText(tr("Enter a username and password to authenticate to %1").arg(QLocale().quoteString(params.value("url").toString())));

    LGConfigPtr config;
    d->repo = params.value("repo").value<LGRepositoryPtr>();
    if (d->repo) {
        config = d->repo->config();
    } else {
        config = LGConfig::defaultConfig();
    }

    d->credentialHelper = config->getString("credential.helper");
    if (d->credentialHelper.isEmpty()) {
        ui->useCredentialHelperButton->setVisible(false);
    }
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

QCoro::Task<> UsernamePasswordSnapIn::on_useCredentialHelperButton_clicked() {
    auto url = QUrl(d->params.value("url").toString());

    QList<QPair<QString, QString>> details;
    details.append({
        {"protocol", url.scheme()},
        {"host", url.host()},
        {"path", url.path()}
    });

    QString process;
    QStringList args;

    if (d->credentialHelper.startsWith("!") || d->credentialHelper.startsWith("/")) {
        args = d->credentialHelper.split(" ");
        process = args.takeFirst();
        if (process.startsWith("!")) process = process.mid(1);
    } else {
        if (!d->repo) co_return;

        process = d->repo->gitExecutable();
        args = d->credentialHelper.split(" ");
        args[0] = "credential-" + args.at(0);
    }

    args.append("get");

    this->setEnabled(false);

    QProcess proc;
    proc.start(process, args);

    for (auto detail : details) {
        proc.write(QStringLiteral("%1=%2\n").arg(detail.first, detail.second).toUtf8());
    }
    proc.closeWriteChannel();
    co_await qCoro(proc).waitForFinished();

    this->setEnabled(true);

    QMap<QString, QString> result;
    while (!proc.atEnd()) {
        auto line = proc.readLine().trimmed();
        int idx = line.indexOf("=");
        if (idx == -1) continue;
        result.insert(line.left(idx), line.mid(idx + 1));
    }

    ui->usernameEdit->setText(result.value("username"));
    ui->passwordEdit->setText(result.value("password"));
}

