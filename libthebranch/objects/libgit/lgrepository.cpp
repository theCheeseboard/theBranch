#include "lgrepository.h"
#include "lgremote.h"

#include "lgactiveremote.h"
#include "lgannotatedtag.h"
#include "lgblob.h"
#include "lgbranch.h"
#include "lgcommit.h"
#include "lgconfig.h"
#include "lgindex.h"
#include "lglightweighttag.h"
#include "lgobject.h"
#include "lgoid.h"
#include "lgreference.h"
#include "lgremote.h"
#include "lgsignature.h"
#include "lgstash.h"
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

LGRepositoryPtr LGRepository::open(QString path) {
    struct git_repository* repo;
    if (git_repository_open(&repo, path.toUtf8().data()) != 0) return nullptr;
    return (new LGRepository(repo))->sharedFromThis();
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

void LGRepository::detachHead(LGCommitPtr commit) {
    git_repository_set_head_detached(d->gitRepository, &commit->oid()->gitOid());
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

LGReferencePtr LGRepository::referenceDwim(QString name) {
    git_reference* gitRef;
    if (git_reference_dwim(&gitRef, d->gitRepository, name.toUtf8().data()) != 0) return nullptr;
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

LGOidPtr LGRepository::createCommit(LGSignaturePtr author, LGSignaturePtr committer, QString message, LGTreePtr tree, QList<LGCommitPtr> parents) {
    return createCommit("HEAD", author, committer, message, tree, parents);
}

LGOidPtr LGRepository::createCommit(QString refToUpdate, LGSignaturePtr author, LGSignaturePtr committer, QString message, LGTreePtr tree, QList<LGCommitPtr> parents) {
    git_oid oid;
    QByteArray updateRefData;
    const char* update_ref;
    if (refToUpdate.isEmpty()) {
        update_ref = nullptr;
    } else {
        // Keep a referenfe to the QByteArray so that it doesn't go out of scope
        updateRefData = refToUpdate.toUtf8();
        update_ref = updateRefData.data();
    }

    const git_commit** commit_parents = new const git_commit*[parents.count()];
    for (int i = 0; i < parents.length(); i++) {
        commit_parents[i] = parents.at(i)->gitCommit();
    }

    if (git_commit_create(&oid, d->gitRepository, update_ref, author->gitSignature(), committer->gitSignature(), "UTF-8", message.toUtf8().data(), tree->gitTree(), parents.count(), commit_parents) != 0) {
        delete[] commit_parents;
        return LGOidPtr();
    }

    return (new LGOid(oid))->sharedFromThis();
}

LGBlobPtr LGRepository::lookupBlob(LGOidPtr oid) {
    git_blob* blob;
    if (git_blob_lookup(&blob, d->gitRepository, &oid->gitOid()) != 0) return nullptr;
    return (new LGBlob(blob))->sharedFromThis();
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

ErrorResponse LGRepository::checkoutTree(LGTreePtr tree, QVariantMap options) {
    ErrorResponse response = performCheckout([this, tree](git_checkout_options* checkoutOptions) {
        return git_checkout_tree(d->gitRepository, lookupObject(tree->oid(), ObjectType::Tree)->gitObject(), checkoutOptions);
    },
        options);
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
    git_strarray_dispose(&remoteNames);
    return remotes;
}

QList<LGTagPtr> LGRepository::tags() {


    struct TagRequestPayload {
            QList<LGTagPtr> tags;
            git_repository* repo;
    };

    TagRequestPayload payload;
    payload.repo = d->gitRepository;

    git_tag_foreach(
        d->gitRepository, [](const char* name, git_oid* oid, void* payload) {
            TagRequestPayload* pl = reinterpret_cast<TagRequestPayload*>(payload);

            git_tag* tag;
            if (git_tag_lookup(&tag, pl->repo, oid) != 0) {
                pl->tags.append((new LGLightweightTag(QString::fromUtf8(name), (new LGOid(oid))->sharedFromThis()))->tbSharedFromThis<LGLightweightTag>::sharedFromThis());
            } else {
                pl->tags.append((new LGAnnotatedTag(tag))->tbSharedFromThis<LGAnnotatedTag>::sharedFromThis());
            }

            return 0;
        },
        &payload);
    return payload.tags;
}

LGTagPtr LGRepository::createLightweightTag(QString name, LGCommitPtr target) {
    auto object = this->lookupObject(target->oid(), ObjectType::Commit);
    if (git_tag_create_lightweight(&target->oid()->gitOid(), d->gitRepository, name.toUtf8().constData(), object.data()->gitObject(), true) != 0) return nullptr;
    return (new LGLightweightTag(name, target->oid()))->tbSharedFromThis<LGLightweightTag>::sharedFromThis();
}

LGRepository::RepositoryState LGRepository::state() {
    git_repository_state_t state = static_cast<git_repository_state_t>(git_repository_state(d->gitRepository));
    switch (state) {
        case GIT_REPOSITORY_STATE_NONE:
            return IdleRepositoryState;
        case GIT_REPOSITORY_STATE_MERGE:
            return MergeRepositoryState;
        case GIT_REPOSITORY_STATE_REVERT:
            return RevertRepositoryState;
        case GIT_REPOSITORY_STATE_REVERT_SEQUENCE:
            return RevertSequenceRepositoryState;
        case GIT_REPOSITORY_STATE_CHERRYPICK:
            return CherryPickRepositoryState;
        case GIT_REPOSITORY_STATE_CHERRYPICK_SEQUENCE:
            return CherryPickSequenceRepositoryState;
        case GIT_REPOSITORY_STATE_BISECT:
            return BisectRepositoryState;
        case GIT_REPOSITORY_STATE_REBASE:
            return RebaseRepositoryState;
        case GIT_REPOSITORY_STATE_REBASE_INTERACTIVE:
            return RebaseInteractiveRepositoryState;
        case GIT_REPOSITORY_STATE_REBASE_MERGE:
            return RebaseMergeRepositoryState;
        case GIT_REPOSITORY_STATE_APPLY_MAILBOX:
            return ApplyMailboxRepositoryState;
        case GIT_REPOSITORY_STATE_APPLY_MAILBOX_OR_REBASE:
            return ApplyMailboxOrRebaseRepositoryState;
    }
    return UnknownRepositoryState;
}

void LGRepository::cleanupState() {
    git_repository_state_cleanup(d->gitRepository);
}

QCoro::Task<> LGRepository::push(QString upstreamRemote, QString upstreamBranch, bool force, bool setUpstream, bool pushTags, InformationRequiredCallback callback) {
    auto headBranch = (new LGBranch(this->head()->takeGitReference()))->sharedFromThis();

    QStringList refs;
    refs.append(QStringLiteral("%1refs/heads/%2:refs/heads/%3").arg(force ? "+" : "", headBranch->name(), upstreamBranch));
    if (pushTags) {
        for (auto tag : this->tags()) {
            refs.append(QStringLiteral("refs/tags/%1:refs/tags/%1").arg(tag->name()));
        }
    }

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

QCoro::Task<> LGRepository::stash(QString message, LGSignaturePtr signature) {
    auto data = message.toUtf8();
    git_oid oid;
    if (git_stash_save(&oid, d->gitRepository, signature->gitSignature(), data.data(), 0) != 0) {
        ErrorResponse::fromCurrentGitError().throwIfError();
    }
    co_return;
}

QList<LGStashPtr> LGRepository::stashes() {
    QList<LGStashPtr> stashes;
    git_stash_foreach(
        d->gitRepository, [](size_t index, const char* message, const git_oid* oid, void* payload) -> int {
            auto stashes = reinterpret_cast<QList<LGStashPtr>*>(payload);
            stashes->append((new LGStash(index, QString::fromUtf8(message)))->sharedFromThis());
            return 0;
        },
        &stashes);
    return stashes;
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

LGObjectPtr LGRepository::lookupObject(LGOidPtr oid, ObjectType type) {
    git_object_t objType;
    switch (type) {
        case ObjectType::Any:
            objType = GIT_OBJ_ANY;
            break;
        case ObjectType::Tree:
            objType = GIT_OBJ_TREE;
            break;
        case ObjectType::Commit:
            objType = GIT_OBJ_COMMIT;
            break;
        case ObjectType::Blob:
            objType = GIT_OBJ_BLOB;
            break;
        case ObjectType::Tag:
            objType = GIT_OBJ_TAG;
            break;
    }

    git_object* gitObj;
    if (git_object_lookup(&gitObj, d->gitRepository, &oid->gitOid(), GIT_OBJ_ANY) != 0) return nullptr;
    return (new LGObject(gitObj))->sharedFromThis();
}

LGObjectPtr LGRepository::revparse(QString revision) {
    git_object* gitObj;
    if (git_revparse_single(&gitObj, d->gitRepository, revision.toUtf8().data()) != 0) return nullptr;
    return (new LGObject(gitObj))->sharedFromThis();
}

ErrorResponse LGRepository::reset(LGObjectPtr object, ResetType resetType) {
    ErrorResponse response = performCheckout([this, object, resetType](git_checkout_options* options) {
        git_reset_t gitResetType;
        switch (resetType) {
            case ResetType::HardReset:
                gitResetType = GIT_RESET_HARD;
                break;
            case ResetType::MixedReset:
                gitResetType = GIT_RESET_MIXED;
                break;
            case ResetType::SoftReset:
                gitResetType = GIT_RESET_SOFT;
                break;
        }

        return git_reset(d->gitRepository, object->gitObject(), gitResetType, options);
    },
        {});
    return response;
}

LGConfigPtr LGRepository::config() {
    git_config* config;
    if (git_repository_config(&config, d->gitRepository) != 0) return nullptr;
    return (new LGConfig(config, false))->sharedFromThis();
}

LGRepository::~LGRepository() {
    git_repository_free(d->gitRepository);
    delete d;
}

LGRepositoryPtr LGRepository::init(QString path) {
    struct git_repository* repo;
    if (git_repository_init(&repo, path.toUtf8().data(), false) != 0) return nullptr;
    return (new LGRepository(repo))->sharedFromThis();
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
