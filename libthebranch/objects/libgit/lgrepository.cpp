#include "lgrepository.h"
#include "lgremote.h"

#include "lgactiveremote.h"
#include "lgbranch.h"
#include "lgcommit.h"
#include "lgindex.h"
#include "lgoid.h"
#include "lgreference.h"
#include "lgremote.h"
#include "lgsignature.h"
#include "lgtree.h"
#include <QCoroProcess>
#include <QException>
#include <QProcess>
#include <QStandardPaths>
#include <QTemporaryFile>
#include <QVariantMap>
#include <git2.h>

struct LGRepositoryPrivate {
        git_repository* gitRepository;
};

LGRepository::LGRepository(struct git_repository* git_repository) :
    QObject{nullptr} {
    d = new LGRepositoryPrivate();
    d->gitRepository = git_repository;
}

LGRepository* LGRepository::open(QString path) {
    struct git_repository* repo;
    if (git_repository_open(&repo, path.toUtf8().data()) != 0) return nullptr;
    return new LGRepository(repo);
}

QString LGRepository::gitExecutable() {
    auto paths = QStandardPaths::findExecutable("git");
    if (paths.isEmpty()) return "";
    return paths;
}

QString LGRepository::path() {
    return QString::fromUtf8(git_repository_path(d->gitRepository));
}

QString LGRepository::workDir() {
    return QString::fromUtf8(git_repository_workdir(d->gitRepository));
}

LGReferencePtr LGRepository::head() {
    git_reference* head;
    if (git_repository_head(&head, d->gitRepository) != 0) return nullptr;
    return (new LGReference(head))->sharedFromThis();
}

void LGRepository::setHead(QString head) {
    git_repository_set_head(d->gitRepository, head.toUtf8().data());
}

QList<LGBranchPtr> LGRepository::branches(THEBRANCH::ListBranchFlags flags) {
    QList<LGBranchPtr> branches;
    git_branch_t gitFlags;
    if (flags == THEBRANCH::AllBranches) gitFlags = GIT_BRANCH_ALL;
    if (flags == THEBRANCH::RemoteBranches) gitFlags = GIT_BRANCH_REMOTE;
    if (flags == THEBRANCH::LocalBranches) gitFlags = GIT_BRANCH_LOCAL;

    git_branch_iterator* iterator;
    if (git_branch_iterator_new(&iterator, d->gitRepository, gitFlags) != 0) return {};

    git_reference* reference;
    git_branch_t type;
    while (!git_branch_next(&reference, &type, iterator)) {
        branches.append((new LGBranch(reference))->sharedFromThis());
    }
    git_branch_iterator_free(iterator);
    return branches;
}

LGBranchPtr LGRepository::createBranch(QString name, LGCommitPtr target) {
    git_reference* ref;
    if (git_branch_create(&ref, d->gitRepository, name.toUtf8().data(), target->gitCommit(), false) != 0) return nullptr;
    return (new LGBranch(ref))->sharedFromThis();
}

LGReferencePtr LGRepository::reference(QString name) {
    git_reference* gitRef;
    if (git_reference_lookup(&gitRef, d->gitRepository, name.toUtf8().data()) != 0) return nullptr;
    return (new LGReference(gitRef))->sharedFromThis();
}

LGIndexPtr LGRepository::index() {
    git_index* index;
    if (git_repository_index(&index, d->gitRepository) != 0) {
        return nullptr;
    }
    return (new LGIndex(index))->sharedFromThis();
}

LGTreePtr LGRepository::lookupTree(LGOidPtr oid) {
    git_tree* tree;
    if (git_tree_lookup(&tree, d->gitRepository, &oid->gitOid()) != 0) return nullptr;
    return (new LGTree(tree))->sharedFromThis();
}

LGCommitPtr LGRepository::lookupCommit(LGOidPtr oid) {
    git_commit* commit;
    if (git_commit_lookup(&commit, d->gitRepository, &oid->gitOid()) != 0) return nullptr;
    return (new LGCommit(commit))->sharedFromThis();
}

#include <tlogger.h>
QCoro::Task<> LGRepository::commit(QString message, LGSignaturePtr committer) {
    // Use git command to write commits
    QTemporaryFile messageFile;
    messageFile.open();
    messageFile.write(message.toUtf8());
    messageFile.close();

    auto args = QStringList({"commit", QStringLiteral("--file=%1").arg(messageFile.fileName()), QStringLiteral("--author=%1 <%2>").arg(committer->name(), committer->email())});
    auto [exitCode, output] = co_await this->runGit(args);

    tDebug("LGRepository") << output;
}

LGSignaturePtr LGRepository::defaultSignature() {
    git_signature* sig;
    if (git_signature_default(&sig, d->gitRepository) != 0) return nullptr;
    return (new LGSignature(sig))->sharedFromThis();
}

ErrorResponse LGRepository::checkoutTree(LGReferencePtr revision, QVariantMap options) {
    git_object* obj;
    if (git_revparse_single(&obj, d->gitRepository, revision->name().toUtf8().data()) != 0) {
        const git_error* err = git_error_last();
        return ErrorResponse(ErrorResponse::UnspecifiedError, err->message);
    }

    ErrorResponse response = performCheckout([=](git_checkout_options* checkoutOptions) {
        return git_checkout_tree(d->gitRepository, obj, checkoutOptions);
    },
        options);

    git_object_free(obj);

    return response;
}

ErrorResponse LGRepository::checkoutIndex(LGIndexPtr index, QVariantMap options) {
    return performCheckout([=](git_checkout_options* checkoutOptions) {
        return git_checkout_index(d->gitRepository, index->gitIndex(), checkoutOptions);
    },
        options);
}

LGRemotePtr LGRepository::createRemote(QString name, QString url) {
    git_remote* remote;
    if (git_remote_create(&remote, d->gitRepository, name.toUtf8().data(), url.toUtf8().data()) != 0) return nullptr;
    git_remote_free(remote);

    return (new LGRemote(name, this->sharedFromThis()))->sharedFromThis();
}

QList<LGRemotePtr> LGRepository::remotes() {
    git_strarray remoteNames;
    git_remote_list(&remoteNames, d->gitRepository);

    QList<LGRemotePtr> remotes;
    for (auto i = 0; i < remoteNames.count; i++) {
        remotes.append((new LGRemote(remoteNames.strings[i], this->sharedFromThis()))->sharedFromThis());
    }
    git_strarray_free(&remoteNames);
    return remotes;
}

LGRepository::RepositoryState LGRepository::state() {
    git_repository_state_t state = static_cast<git_repository_state_t>(git_repository_state(d->gitRepository));
    switch (state) {
        case GIT_REPOSITORY_STATE_NONE:
            return IdleRepositoryState;
        case GIT_REPOSITORY_STATE_MERGE:
            return MergeRepositoryState;
    }
    return UnknownRepositoryState;
}

void LGRepository::cleanupState() {
    git_repository_state_cleanup(d->gitRepository);
}

QCoro::Task<> LGRepository::push(QString upstreamRemote, QString upstreamBranch, bool setUpstream, bool pushTags, InformationRequiredCallback callback) {
    auto headBranch = (new LGBranch(this->head()->takeGitReference()))->sharedFromThis();

    QStringList refs;
    refs.append(QStringLiteral("refs/heads/%1:refs/heads/%2").arg(headBranch->name(), upstreamBranch));
    if (pushTags) refs.append("refs/tags/*:refs/tags/*");

    co_await this->push(upstreamRemote, refs, callback);

    if (setUpstream) {
        for (auto branch : this->branches(THEBRANCH::RemoteBranches)) {
            if (branch->name() == QStringLiteral("%1/%2").arg(upstreamRemote, upstreamBranch)) {
                headBranch->setUpstream(branch);
            }
        }
    }
}

QCoro::Task<> LGRepository::push(QString upstreamRemote, QStringList refs, InformationRequiredCallback callback) {
    auto rem = (new LGRemote(upstreamRemote, this->sharedFromThis()))->sharedFromThis();
    if (!rem) throw QException();

    auto activeRem = rem->activeRemote();
    activeRem->setInformationRequiredCallback(callback);
    co_await activeRem->connect(true);
    co_await activeRem->push(refs);
}

QCoro::Task<> LGRepository::fetch(QString remote, QStringList refs, InformationRequiredCallback callback) {
    auto rem = (new LGRemote(remote, this->sharedFromThis()))->sharedFromThis();
    if (!rem) throw QException();

    auto activeRem = rem->activeRemote();
    activeRem->setInformationRequiredCallback(callback);
    co_await activeRem->connect(false);
    co_await activeRem->fetch(refs);
}

QCoro::Task<std::tuple<int, QString>> LGRepository::runGit(QStringList args) {
    auto gitCommand = this->gitExecutable();
    if (gitCommand.isEmpty()) throw QException();

    QProcess gitProc;
    gitProc.setWorkingDirectory(this->workDir());
    gitProc.setProcessChannelMode(QProcess::MergedChannels);
    co_await qCoro(gitProc).start(gitCommand, args);
    co_await qCoro(gitProc).waitForFinished();

    co_return {gitProc.exitCode(), gitProc.readAll()};
}

LGRepository::~LGRepository() {
    git_repository_free(d->gitRepository);
    delete d;
}

git_repository* LGRepository::gitRepository() {
    return d->gitRepository;
}

ErrorResponse LGRepository::performCheckout(std::function<int(git_checkout_options*)> specificCheckout, QVariantMap options) {
    git_checkout_options* checkoutOptions = new git_checkout_options();
    git_checkout_options_init(checkoutOptions, GIT_CHECKOUT_OPTIONS_VERSION);

    struct NotifyDetails {
            QStringList conflicts;
    };
    NotifyDetails* details = new NotifyDetails();

    checkoutOptions->checkout_strategy = GIT_CHECKOUT_SAFE;
    if (options.value("force", false).toBool()) checkoutOptions->checkout_strategy = GIT_CHECKOUT_FORCE;

    checkoutOptions->notify_cb = [](
                                     git_checkout_notify_t why,
                                     const char* path,
                                     const git_diff_file* baseline,
                                     const git_diff_file* target,
                                     const git_diff_file* workdir,
                                     void* payload) {
        NotifyDetails* details = reinterpret_cast<NotifyDetails*>(payload);
        if (why == GIT_CHECKOUT_NOTIFY_CONFLICT) {
            details->conflicts.append(QString::fromUtf8(path));
        }
        return 0;
    };
    checkoutOptions->notify_flags = GIT_CHECKOUT_NOTIFY_ALL;
    checkoutOptions->notify_payload = details;

    if (specificCheckout(checkoutOptions) != 0) {
        const git_error* err = git_error_last();
        ErrorResponse resp = ErrorResponse(ErrorResponse::UnspecifiedError, err->message, {
                                                                                              {"conflicts", details->conflicts}
        });
        delete details;
        return resp;
    }
    delete details;

    delete checkoutOptions;
    return ErrorResponse();
}
