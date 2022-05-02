#include "repositorycloneoperation.h"

#include <QProcess>

struct RepositoryCloneOperationPrivate {
        Repository::RepositoryState state = Repository::Cloning;
        QString cloneOutput;
};

RepositoryCloneOperation::RepositoryCloneOperation(QString cloneUrl, QString directory, QVariantMap options, QObject* parent) :
    RepositoryOperation{parent} {
    d = new RepositoryCloneOperationPrivate;
    d->cloneOutput = tr("Preparing to clone...").append("\n");

    QStringList args = {
        "clone",
        cloneUrl,
        directory};

    QProcess* cloneProcess = new QProcess();
    cloneProcess->setProcessChannelMode(QProcess::MergedChannels);
    connect(cloneProcess, &QProcess::readyRead, this, [=] {
        d->cloneOutput.append(cloneProcess->readAll());
        emit stateInformationalTextChanged();
    });
    connect(cloneProcess, &QProcess::finished, this, [=](int exitCode, QProcess::ExitStatus exitStatus) {
        if (exitCode == 0) {
            emit reloadRepository();
            emit done();
        } else {
            d->state = Repository::Invalid;
            emit stateChanged();
            emit stateDescriptionChanged();
            emit progressChanged();
        }
    });
    cloneProcess->start(theBranch::gitExecutable(), args);
}

RepositoryCloneOperation::~RepositoryCloneOperation() {
    delete d;
}

Repository::RepositoryState RepositoryCloneOperation::state() {
    return d->state;
}

QString RepositoryCloneOperation::stateDescription() {
    if (d->state == Repository::Invalid) {
        return tr("Repository Clone failed");
    }
    return tr("Cloning Repository...");
}

QString RepositoryCloneOperation::stateInformationalText() {
    QStringList cloneOutput = d->cloneOutput.split("\n");
    cloneOutput.removeAll("");
    return cloneOutput.last();
}

int RepositoryCloneOperation::progress() {
    return 0;
}

int RepositoryCloneOperation::totalProgress() {
    return 0;
}

bool RepositoryCloneOperation::providesProgress() {
    if (d->state == Repository::Invalid) {
        return false;
    }
    return true;
}
