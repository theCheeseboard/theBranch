#include "repository.h"

#include "private/repositorycloneoperation.h"
#include <QFileDialog>
#include <git2.h>
#include <tmessagebox.h>

struct RepositoryPrivate {
        git_repository* gitRepo = nullptr;
        Repository::RepositoryState state = Repository::Invalid;

        QList<RepositoryOperation*> operations;
};

Repository::Repository(QObject* parent) :
    QObject{parent} {
    d = new RepositoryPrivate;
}

void Repository::putRepositoryOperation(RepositoryOperation* operation) {
    d->operations.append(operation);
    connect(operation, &RepositoryOperation::done, this, [=] {
        emit stateChanged();
        emit stateDescriptionChanged();
        emit stateInformationalTextChanged();
        emit stateProgressChanged();
        d->operations.removeAll(operation);
    });
    connect(operation, &RepositoryOperation::stateChanged, this, &Repository::stateChanged);
    connect(operation, &RepositoryOperation::stateDescriptionChanged, this, &Repository::stateDescriptionChanged);
    connect(operation, &RepositoryOperation::stateInformationalTextChanged, this, &Repository::stateInformationalTextChanged);
    connect(operation, &RepositoryOperation::progressChanged, this, &Repository::stateProgressChanged);
    connect(operation, &RepositoryOperation::reloadRepository, this, &Repository::reloadRepositoryState);
}

Repository::~Repository() {
    delete d;
}

Repository::RepositoryState Repository::state() {
    if (!d->operations.isEmpty()) return d->operations.first()->state();
    return d->state;
}

QString Repository::stateDescription() {
    if (!d->operations.isEmpty()) return d->operations.first()->stateDescription();
    return "State Description";
}

QString Repository::stateInformationalText() {
    if (!d->operations.isEmpty()) return d->operations.first()->stateInformationalText();
    return "State Informational Text";
}

int Repository::stateProgress() {
    if (!d->operations.isEmpty()) return d->operations.first()->progress();
    return 0;
}

int Repository::stateTotalProgress() {
    if (!d->operations.isEmpty()) return d->operations.first()->totalProgress();
    return 0;
}

bool Repository::stateProvidesProgress() {
    if (!d->operations.isEmpty()) return d->operations.first()->providesProgress();
    return false;
}

Repository* Repository::cloneRepository(QString cloneUrl, QString directory, QVariantMap options) {
    RepositoryOperation* operation = new RepositoryCloneOperation(cloneUrl, directory, options);

    Repository* repo = new Repository();
    repo->putRepositoryOperation(operation);
    git_repository_open(&repo->d->gitRepo, directory.toUtf8().data());
    return repo;
}

tPromise<Repository*>* Repository::repositoryForDirectoryUi(QWidget* parent) {
    return TPROMISE_CREATE_SAME_THREAD(Repository*, {
        QFileDialog* dialog = new QFileDialog(parent);
        dialog->setAcceptMode(QFileDialog::AcceptOpen);
        dialog->setFileMode(QFileDialog::Directory);
        connect(
            dialog, &QFileDialog::accepted, parent, [=] {
                Repository* repo = Repository::repositoryForDirectory(dialog->selectedFiles().first());
                if (repo) {
                    res(repo);
                } else {
                    tMessageBox* box = new tMessageBox(parent->window());
                    box->setTitleBarText(tr("No Git Repository Available"));
                    box->setMessageText(tr("The folder that you selected does not contain a Git repository. Do you want to create an empty Git repository there?"));
                    box->setIcon(QMessageBox::Question);
                    tMessageBoxButton* createButton = box->addButton(tr("Create and Open Git Repository"), QMessageBox::AcceptRole);
                    tMessageBoxButton* cancelButton = box->addStandardButton(QMessageBox::Cancel);

                    connect(createButton, &tMessageBoxButton::buttonPressed, parent, [=] {
                        rej("Not Implemented");
                    });
                    connect(cancelButton, &tMessageBoxButton::buttonPressed, parent, [=] {
                        rej("Cancelled");
                    });

                    box->exec(true);
                }
            },
            Qt::QueuedConnection);
        connect(dialog, &QFileDialog::finished, dialog, &QFileDialog::deleteLater);
        dialog->open();
    });
}

Repository* Repository::repositoryForDirectory(QString directory) {
    git_buf buf = GIT_BUF_INIT;
    int retval = git_repository_discover(&buf, directory.toUtf8().data(), false, nullptr);

    if (retval != 0) return nullptr;
    QString path = QString::fromUtf8(buf.ptr, buf.size);

    git_buf_dispose(&buf);

    Repository* repo = new Repository();
    git_repository_open(&repo->d->gitRepo, path.toUtf8().data());
    repo->reloadRepositoryState();
    return repo;
}

QString Repository::gitRepositoryRootForDirectory(QString directory) {
    return "";
}

void Repository::reloadRepositoryState() {
    if (!d->gitRepo) {
        d->state = Invalid;
        emit stateChanged();
        return;
    }

    d->state = Idle;
    emit stateChanged();
}

QString Repository::repositoryPath() {
    QString gitFolder = QString::fromUtf8(git_repository_path(d->gitRepo));
    return QDir::cleanPath(QDir(gitFolder).absoluteFilePath(".."));
}
