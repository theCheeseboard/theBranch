#include "repositorycloneoperation.h"

#include "../libgit/lgclone.h"
#include "../libgit/lgrepository.h"
#include <QProcess>

struct RepositoryCloneOperationPrivate {
        Repository::RepositoryState state = Repository::Cloning;
        QString cloneOutput;

        int progress = 0;
        int totalProgress = 0;
};

RepositoryCloneOperation::RepositoryCloneOperation(QString cloneUrl, QString directory, QVariantMap options, QObject* parent) :
    RepositoryOperation{parent} {
    d = new RepositoryCloneOperationPrivate;
    d->cloneOutput = tr("Preparing to clone...").append("\n");

    //    QStringList args = {
    //        "clone",
    //        cloneUrl,
    //        directory};

    //    QProcess* cloneProcess = new QProcess();
    //    cloneProcess->setProcessChannelMode(QProcess::MergedChannels);
    //    connect(cloneProcess, &QProcess::readyRead, this, [=] {
    //        d->cloneOutput.append(cloneProcess->readAll());
    //        emit stateInformationalTextChanged();
    //    });
    //    connect(cloneProcess, &QProcess::finished, this, [=](int exitCode, QProcess::ExitStatus exitStatus) {
    //        if (exitCode == 0) {
    //            emit putRepository(LGRepositoryPtr(LGRepository::open(directory)));
    //            emit done();
    //        } else {
    //            d->state = Repository::Invalid;
    //            emit stateChanged();
    //            emit stateDescriptionChanged();
    //            emit progressChanged();
    //        }
    //    });
    //    cloneProcess->start(theBranch::gitExecutable(), args);

    LGClone* clone = new LGClone();
    connect(clone, &LGClone::progressChanged, this, [=](int bytesTransferred, int objectsReceived, int totalObjects) {
        d->progress = objectsReceived;
        d->totalProgress = totalObjects;
        d->cloneOutput = tr("Receiving objects: %1/%2 - %3 transferred.").arg(objectsReceived).arg(totalObjects).arg(QLocale().formattedDataSize(bytesTransferred));
        emit progressChanged();
        emit stateInformationalTextChanged();
    });
    connect(clone, &LGClone::statusChanged, this, [=](QString description) {
        d->cloneOutput = description.trimmed();
        emit stateInformationalTextChanged();
    });
    clone->clone(cloneUrl, directory).then([=] {
        emit putRepository(LGRepositoryPtr(LGRepository::open(directory)));
        emit done();
    },
        [=](const std::exception e) {
        //            d->cloneOutput = error;
        d->state = Repository::Invalid;
        emit stateChanged();
        emit stateDescriptionChanged();
        emit stateInformationalTextChanged();
        emit progressChanged();
    });
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
    //    QStringList cloneOutput = d->cloneOutput.split("\n");
    //    cloneOutput.removeAll("");
    //    return cloneOutput.last();
    return d->cloneOutput;
}

int RepositoryCloneOperation::progress() {
    return d->progress;
}

int RepositoryCloneOperation::totalProgress() {
    return d->totalProgress;
}

bool RepositoryCloneOperation::providesProgress() {
    if (d->state == Repository::Invalid) {
        return false;
    }
    return true;
}
