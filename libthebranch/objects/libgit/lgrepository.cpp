#include "lgrepository.h"

#include "lgbranch.h"
#include "lgindex.h"
#include "lgreference.h"
#include <QVariantMap>
#include <git2.h>

struct LGRepositoryPrivate {
        git_repository* git_repository;
};

LGRepository::LGRepository(struct git_repository* git_repository) :
    QObject{nullptr} {
    d = new LGRepositoryPrivate();
    d->git_repository = git_repository;
}

LGRepository* LGRepository::open(QString path) {
    struct git_repository* repo;
    if (git_repository_open(&repo, path.toUtf8().data()) != 0) return nullptr;
    return new LGRepository(repo);
}

QString LGRepository::path() {
    return QString::fromUtf8(git_repository_path(d->git_repository));
}

LGReferencePtr LGRepository::head() {
    git_reference* head;
    if (git_repository_head(&head, d->git_repository) != 0) return nullptr;
    return LGReferencePtr(new LGReference(head));
}

void LGRepository::setHead(QString head) {
    git_repository_set_head(d->git_repository, head.toUtf8().data());
}

QList<LGBranchPtr> LGRepository::branches(THEBRANCH::ListBranchFlags flags) {
    QList<LGBranchPtr> branches;
    git_branch_t gitFlags;
    if (flags == THEBRANCH::AllBranches) gitFlags = GIT_BRANCH_ALL;
    if (flags == THEBRANCH::RemoteBranches) gitFlags = GIT_BRANCH_REMOTE;
    if (flags == THEBRANCH::LocalBranches) gitFlags = GIT_BRANCH_LOCAL;

    git_branch_iterator* iterator;
    if (git_branch_iterator_new(&iterator, d->git_repository, gitFlags) != 0) return {};

    git_reference* reference;
    git_branch_t type;
    while (!git_branch_next(&reference, &type, iterator)) {
        branches.append(LGBranchPtr(new LGBranch(reference)));
    }
    git_branch_iterator_free(iterator);
    return branches;
}

LGIndexPtr LGRepository::index() {
    git_index* index;
    if (git_repository_index(&index, d->git_repository) != 0) {
        return nullptr;
    }
    return LGIndexPtr(new LGIndex(index));
}

ErrorResponse LGRepository::checkoutTree(LGReferencePtr revision, QVariantMap options) {
    git_object* obj;
    if (git_revparse_single(&obj, d->git_repository, revision->name().toUtf8().data()) != 0) {
        const git_error* err = git_error_last();
        return ErrorResponse(ErrorResponse::UnspecifiedError, err->message);
    }

    ErrorResponse response = performCheckout([=](git_checkout_options* checkoutOptions) {
        return git_checkout_tree(d->git_repository, obj, checkoutOptions);
    },
        options);

    git_object_free(obj);

    return response;
}

ErrorResponse LGRepository::checkoutIndex(LGIndexPtr index, QVariantMap options) {
    return performCheckout([=](git_checkout_options* checkoutOptions) {
        return git_checkout_index(d->git_repository, index->git_index(), checkoutOptions);
    },
        options);
}

LGRepository::RepositoryState LGRepository::state() {
    git_repository_state_t state = static_cast<git_repository_state_t>(git_repository_state(d->git_repository));
    switch (state) {
        case GIT_REPOSITORY_STATE_NONE:
            return IdleRepositoryState;
        case GIT_REPOSITORY_STATE_MERGE:
            return MergeRepositoryState;
    }
    return UnknownRepositoryState;
}

void LGRepository::cleanupState() {
    git_repository_state_cleanup(d->git_repository);
}

LGRepository::~LGRepository() {
    git_repository_free(d->git_repository);
    delete d;
}

git_repository* LGRepository::git_repository() {
    return d->git_repository;
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
