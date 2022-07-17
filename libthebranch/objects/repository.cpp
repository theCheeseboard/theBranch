#include "repository.h"

#include "branch.h"
#include "commit.h"
#include "index.h"
#include "libgit/lgbranch.h"
#include "libgit/lgcommit.h"
#include "libgit/lgreference.h"
#include "libgit/lgrepository.h"
#include "private/repositorycloneoperation.h"
#include "reference.h"
#include "remote.h"
#include <QCoroSignal>
#include <QDirIterator>
#include <QFileDialog>
#include <QFileSystemWatcher>
#include <git2.h>
#include <tmessagebox.h>

struct RepositoryPrivate {
        LGRepositoryPtr gitRepo;
        Repository::RepositoryState state = Repository::Invalid;

        QList<RepositoryOperation*> operations;
        QFileSystemWatcher* watcher;
        QTimer* watcherTimer;
};

Repository::Repository(QObject* parent) :
    QObject{parent} {
    d = new RepositoryPrivate;

    d->watcherTimer = new QTimer();
    d->watcherTimer->setInterval(500);
    d->watcherTimer->setSingleShot(true);
    connect(d->watcherTimer, &QTimer::timeout, this, [this] {
        updateWatchedDirectories();
        emit repositoryUpdated();
    });

    d->watcher = new QFileSystemWatcher();
    connect(d->watcher, &QFileSystemWatcher::directoryChanged, this, [this] {
        d->watcherTimer->stop();
        d->watcherTimer->start();
    });
}

void Repository::putRepositoryOperation(RepositoryOperation* operation) {
    d->operations.append(operation);
    connect(operation, &RepositoryOperation::done, this, [this, operation] {
        emit stateChanged();
        emit stateDescriptionChanged();
        emit stateInformationalTextChanged();
        emit stateProgressChanged();
        d->operations.removeAll(operation);

        reloadRepositoryState();
    });
    connect(operation, &RepositoryOperation::stateChanged, this, &Repository::stateChanged);
    connect(operation, &RepositoryOperation::stateDescriptionChanged, this, &Repository::stateDescriptionChanged);
    connect(operation, &RepositoryOperation::stateInformationalTextChanged, this, &Repository::stateInformationalTextChanged);
    connect(operation, &RepositoryOperation::progressChanged, this, &Repository::stateProgressChanged);
    connect(operation, &RepositoryOperation::putRepository, this, [this](LGRepositoryPtr repository) {
        d->gitRepo = repository;
        reloadRepositoryState();
    });
    connect(operation, &RepositoryOperation::reloadRepository, this, &Repository::reloadRepositoryState);
}

void Repository::updateWatchedDirectories() {
    QDirIterator iterator(d->gitRepo->path(), QDirIterator::Subdirectories);
    while (iterator.hasNext()) {
        iterator.next();
        if (iterator.fileInfo().isDir()) d->watcher->addPath(iterator.filePath());
    }
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

ReferencePtr Repository::head() {
    if (!d->gitRepo) return nullptr;
    LGReferencePtr ref = d->gitRepo->head();
    if (!ref) return nullptr;
    return Reference::referenceForLgReference(d->gitRepo, ref);
}

QList<BranchPtr> Repository::branches(THEBRANCH::ListBranchFlags flags) {
    QList<BranchPtr> branches;
    for (LGBranchPtr branch : d->gitRepo->branches(flags)) {
        branches.append(Branch::branchForLgBranch(d->gitRepo, branch)->sharedFromThis());
    }
    return branches;
}

BranchPtr Repository::createBranch(QString name, CommitPtr target) {
    auto branch = d->gitRepo->createBranch(name, target->gitCommit());
    if (!branch) return nullptr;
    return Branch::branchForLgBranch(d->gitRepo, branch);
}

ReferencePtr Repository::reference(QString name) {
    LGReferencePtr ref = d->gitRepo->reference(name);
    if (!ref) return nullptr;
    return Reference::referenceForLgReference(d->gitRepo, ref);
}

ErrorResponse Repository::setHeadAndCheckout(ReferencePtr reference) {
    if (BranchPtr branch = reference->asBranch()) {
        if (branch->isRemoteBranch()) {
            LGReferencePtr ref = d->gitRepo->reference("refs/heads/" + branch->localBranchName());
            if (ref) {
                return ErrorResponse(ErrorResponse::UnspecifiedError, tr("The local branch %1 already exists.").arg(QLocale().quoteString(branch->localBranchName())));
            } else {
                // There is no local branch for this remote
                // Create a local branch and track the remote branch
                LGCommitPtr commit = LGCommit::lookup(d->gitRepo, LGReference::nameToId(d->gitRepo, reference->name()));
                LGBranchPtr newBranch = d->gitRepo->createBranch(branch->localBranchName(), commit);
                if (!newBranch) {
                    return ErrorResponse(ErrorResponse::UnspecifiedError, "Could not create local branch");
                }

                if (!newBranch->setUpstream(branch->gitBranch())) {
                    return ErrorResponse::fromCurrentGitError();
                }
                reference = Reference::referenceForLgReference(d->gitRepo, (new LGReference(newBranch->dup()->takeGitReference()))->sharedFromThis());
            }
        }
    }

    if (CHK_ERR(d->gitRepo->checkoutTree(reference->git_reference(), {}))) return error;

    d->gitRepo->setHead(reference->name());
    return ErrorResponse();
}

RepositoryPtr Repository::cloneRepository(QString cloneUrl, QString directory, QVariantMap options) {
    RepositoryOperation* operation = new RepositoryCloneOperation(cloneUrl, directory, options);

    Repository* repo = new Repository();
    repo->putRepositoryOperation(operation);
    return repo->sharedFromThis();
}

QCoro::Task<RepositoryPtr> Repository::repositoryForDirectoryUi(QWidget* parent) {
    QFileDialog* dialog = new QFileDialog(parent);
    dialog->setAcceptMode(QFileDialog::AcceptOpen);
    dialog->setFileMode(QFileDialog::Directory);
    dialog->setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    dialog->open();

    const auto result = co_await qCoro(dialog, &QFileDialog::finished);
    dialog->deleteLater();
    if (result == QFileDialog::Rejected) throw QException();

    RepositoryPtr repo = Repository::repositoryForDirectory(dialog->selectedFiles().first());
    if (repo) {
        co_return repo;
    } else {
        tMessageBox* box = new tMessageBox(parent->window());
        box->setTitleBarText(tr("No Git Repository Available"));
        box->setMessageText(tr("The folder that you selected does not contain a Git repository. Do you want to create an empty Git repository there?"));
        box->setIcon(QMessageBox::Question);
        tMessageBoxButton* createButton = box->addButton(tr("Create and Open Git Repository"), QMessageBox::AcceptRole);
        tMessageBoxButton* cancelButton = box->addStandardButton(QMessageBox::Cancel);

        bool create = false;
        connect(createButton, &tMessageBoxButton::buttonPressed, parent, [&create] {
            create = true;
        });
        connect(cancelButton, &tMessageBoxButton::buttonPressed, parent, [=] {
        });

        box->exec(true);

        if (create) {
            throw QException();
        } else {
            throw QException();
        }
    }
}

RepositoryPtr Repository::repositoryForDirectory(QString directory) {
    git_buf buf = GIT_BUF_INIT;
    int retval = git_repository_discover(&buf, directory.toUtf8().data(), false, nullptr);

    if (retval != 0) return nullptr;
    QString path = QString::fromUtf8(buf.ptr, buf.size);

    git_buf_dispose(&buf);

    Repository* repo = new Repository();
    repo->d->gitRepo.reset(LGRepository::open(path));
    repo->reloadRepositoryState();
    return repo->sharedFromThis();
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
    updateWatchedDirectories();
    emit repositoryUpdated();
}

IndexPtr Repository::index() {
    return Index::indexForLgIndex(d->gitRepo->index());
}

QList<Repository::StatusItem> Repository::fileStatus() {
    QList<StatusItem> items;

    git_status_options options;
    git_status_options_init(&options, GIT_STATUS_OPTIONS_VERSION);
    options.show = GIT_STATUS_SHOW_INDEX_AND_WORKDIR;

    git_status_list* statusList;
    if (git_status_list_new(&statusList, d->gitRepo->gitRepository(), &options) != 0) {
        return {};
    }

    git_status_foreach(
        d->gitRepo->gitRepository(), [](const char* path, unsigned int flags, void* payload) {
            QList<StatusItem>* items = reinterpret_cast<QList<StatusItem>*>(payload);

            StatusItem item;
            item.path = QString::fromUtf8(path);
            if (flags & GIT_STATUS_CURRENT) item.flags |= Repository::StatusItem::Current;
            if (flags & GIT_STATUS_WT_NEW || flags & GIT_STATUS_INDEX_NEW) item.flags |= Repository::StatusItem::New;
            if (flags & GIT_STATUS_WT_MODIFIED || flags & GIT_STATUS_INDEX_MODIFIED) item.flags |= Repository::StatusItem::Modified;
            if (flags & GIT_STATUS_WT_DELETED || flags & GIT_STATUS_INDEX_DELETED) item.flags |= Repository::StatusItem::Deleted;
            if (flags & GIT_STATUS_WT_TYPECHANGE || flags & GIT_STATUS_INDEX_TYPECHANGE) item.flags |= Repository::StatusItem::TypeChanged;
            if (flags & GIT_STATUS_WT_RENAMED || flags & GIT_STATUS_INDEX_RENAMED) item.flags |= Repository::StatusItem::Renamed;
            if (flags & GIT_STATUS_CONFLICTED) item.flags |= Repository::StatusItem::Conflicting;
            items->append(item);

            return 0;
        },
        &items);

    git_status_list_free(statusList);
    return items;
}

RemotePtr Repository::addRemote(QString name, QString url) {
    auto remote = d->gitRepo->createRemote(name, url);
    if (!remote) return nullptr;
    return Remote::remoteForLgRemote(remote)->sharedFromThis();
}

QList<RemotePtr> Repository::remotes() {
    QList<RemotePtr> remotes;
    for (auto remote : d->gitRepo->remotes()) {
        remotes.append(Remote::remoteForLgRemote(remote)->sharedFromThis());
    }
    return remotes;
}

QCoro::Task<> Repository::fetch(QString remote) {
    co_return co_await d->gitRepo->fetch(remote);
}

QString Repository::repositoryPath() {
    return QDir::cleanPath(QDir(d->gitRepo->path()).absoluteFilePath(".."));
}

LGRepositoryPtr Repository::git_repository() {
    return d->gitRepo;
}
