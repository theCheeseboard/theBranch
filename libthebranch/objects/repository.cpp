#include "repository.h"

#include "branch.h"
#include "commit.h"
#include "index.h"
#include "libgit/lgbranch.h"
#include "libgit/lgcommit.h"
#include "libgit/lgobject.h"
#include "libgit/lgreference.h"
#include "libgit/lgrepository.h"
#include "objects/blob.h"
#include "private/repositorycloneoperation.h"
#include "reference.h"
#include "remote.h"
#include "stash.h"
#include "tag.h"
#include "tree.h"
#include <QCoroSignal>
#include <QDirIterator>
#include <QFileDialog>
#include <QFileSystemWatcher>
#include <commandpalettes/branchescommandpalette.h>
#include <git2.h>
#include <ranges/trange.h>
#include <tmessagebox.h>

struct RepositoryPrivate {
        LGRepositoryPtr gitRepo;
        Repository::RepositoryState state = Repository::Invalid;

        QList<RepositoryOperation*> operations;
        QFileSystemWatcher* watcher;
        QTimer* watcherTimer;

        BranchesCommandPalette* branchesCommandPalette;
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

    d->branchesCommandPalette = new BranchesCommandPalette(this);
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

Repository::GitState Repository::gitState() {
    if (!d->gitRepo) return GitState::Unknown;
    return static_cast<GitState>(d->gitRepo->state());
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
    if (!d->gitRepo) return {};

    return tRange(d->gitRepo->branches(flags)).map<BranchPtr>([this](const LGBranchPtr& branch) {
                                                  return Branch::branchForLgBranch(d->gitRepo, branch)->sharedFromThis();
                                              })
        .toList();
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

CommitPtr Repository::searchCommit(QString name) {
    LGReferencePtr ref = d->gitRepo->referenceDwim(name);
    if (ref) {
        return Reference::referenceForLgReference(d->gitRepo, ref)->asCommit();
    }

    auto obj = d->gitRepo->revparse(name);
    if (obj) {
        return Commit::commitForLgCommit(d->gitRepo, d->gitRepo->lookupCommit(obj->oid()));
    }
    return nullptr;
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

ErrorResponse Repository::detachHead(CommitPtr target) {
    if (CHK_ERR(d->gitRepo->checkoutTree(target->tree()->gitTree(), {}))) return error;

    d->gitRepo->detachHead(target->gitCommit());
    return ErrorResponse();
}

void Repository::resetFileToHead(QString file) {
    auto head = this->head();
    if (!head) return;

    QFileInfo fileInfo(QDir(this->repositoryPath()).absoluteFilePath(file));
    auto tree = head->asCommit()->tree();
    auto blob = tree->blobForPath(file);

    if (blob) {
        fileInfo.dir().mkpath(".");

        QFile f(fileInfo.filePath());
        f.open(QFile::WriteOnly);
        f.write(blob->contents());
        f.close();
    } else {
        QFile::remove(fileInfo.filePath());
    }
}

RepositoryPtr Repository::cloneRepository(QString cloneUrl, QString directory, InformationRequiredCallback callback, QVariantMap options) {
    RepositoryOperation* operation = new RepositoryCloneOperation(cloneUrl, directory, callback, options);

    Repository* repo = new Repository();
    repo->putRepositoryOperation(operation);
    return repo->sharedFromThis();
}

QCoro::Task<RepositoryPtr> Repository::repositoryForDirectoryUi(QWidget* parent) {
    QFileDialog dialog(parent);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    dialog.open();

    const auto result = co_await qCoro(&dialog, &QFileDialog::finished);
    if (result == QFileDialog::Rejected) throw QException();
    QString path = dialog.selectedFiles().first();

    RepositoryPtr repo = Repository::repositoryForDirectory(path);
    if (repo) {
        co_return repo;
    } else {
        tMessageBox box(parent->window());
        box.setTitleBarText(tr("No Git Repository Available"));
        box.setMessageText(tr("The folder that you selected does not contain a Git repository. Do you want to create an empty Git repository there?"));
        box.setIcon(QMessageBox::Question);
        tMessageBoxButton* createButton = box.addButton(tr("Create and Open Git Repository"), QMessageBox::AcceptRole);
        tMessageBoxButton* cancelButton = box.addStandardButton(QMessageBox::Cancel);

        auto clickedButton = co_await box.presentAsync();

        if (clickedButton == cancelButton) {
            throw QException();
        }

        // Create repository at that path
        repo = Repository::repositoryForInit(path);
        if (!repo) {
            ErrorResponse err = ErrorResponse::fromCurrentGitError();

            tMessageBox box(parent->window());
            box.setTitleBarText(tr("Could not create Git repository"));
            box.setMessageText(err.description());
            box.setIcon(QMessageBox::Critical);
            co_await box.presentAsync();
            throw QException();
        }

        co_return repo;
    }
}

RepositoryPtr Repository::repositoryForDirectory(QString directory) {
    git_buf buf = GIT_BUF_INIT;
    int retval = git_repository_discover(&buf, directory.toUtf8().data(), false, nullptr);

    if (retval != 0) return nullptr;
    QString path = QString::fromUtf8(buf.ptr, buf.size);

    git_buf_dispose(&buf);

    Repository* repo = new Repository();
    repo->d->gitRepo = LGRepository::open(path);
    repo->reloadRepositoryState();
    return repo->sharedFromThis();
}

RepositoryPtr Repository::repositoryForInit(QString directory) {
    auto lgrepo = LGRepository::init(directory);
    if (!lgrepo) return nullptr;

    Repository* repo = new Repository();
    repo->d->gitRepo = lgrepo;
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
    return Remote::remoteForLgRemote(d->gitRepo, remote)->sharedFromThis();
}

QList<RemotePtr> Repository::remotes() {
    if (!d->gitRepo) return {};

    QList<RemotePtr> remotes;
    for (auto remote : d->gitRepo->remotes()) {
        remotes.append(Remote::remoteForLgRemote(d->gitRepo, remote)->sharedFromThis());
    }
    return remotes;
}

tRange<TagPtr> Repository::tags() {
    return tRange(d->gitRepo->tags()).map<TagPtr>([this](LGTagPtr tag) {
        return Tag::tagForLgTag(d->gitRepo, tag);
    });
}

QCoro::Task<> Repository::fetch(QString remote, QStringList refs, InformationRequiredCallback callback) {
    co_return co_await d->gitRepo->fetch(remote, refs, callback);
}

QCoro::Task<> Repository::stash(QString message) {
    co_await d->gitRepo->stash(message, this->git_repository()->defaultSignature());
}

QList<StashPtr> Repository::stashes() {
    if (!d->gitRepo) return {};

    QList<StashPtr> stashes;
    for (auto stash : d->gitRepo->stashes()) {
        stashes.append(Stash::stashForLgStash(stash, d->gitRepo)->sharedFromThis());
    }
    return stashes;
}

ErrorResponse Repository::reset(CommitPtr commit, ResetType type) {
    auto object = d->gitRepo->lookupObject(commit->gitCommit()->oid(), LGRepository::ObjectType::Commit);
    return d->gitRepo->reset(object, static_cast<LGRepository::ResetType>(type));
}

QString Repository::repositoryPath() {
    return QDir::cleanPath(QDir(d->gitRepo->path()).absoluteFilePath(".."));
}

tCommandPaletteScope* Repository::commandPaletteBranches() {
    return d->branchesCommandPalette;
}

LGRepositoryPtr Repository::git_repository() {
    return d->gitRepo;
}

CommitPtr Repository::resolveToCommit() {
    if (auto head = this->head()) return head->resolveToCommit();
    return nullptr;
}
