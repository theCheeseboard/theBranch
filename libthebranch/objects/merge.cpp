#include "merge.h"

#include "commit.h"
#include "libgit/lgannotatedcommit.h"
#include "libgit/lgcommit.h"
#include "libgit/lgindex.h"
#include "libgit/lgreference.h"
#include "libgit/lgrepository.h"
#include "reference.h"
#include "repository.h"
#include <git2.h>

struct MergePrivate {
        git_merge_analysis_t mergeAnalysis;
        git_merge_preference_t mergePreference;

        RepositoryPtr repo;
        BranchPtr fromBranch;
        LGAnnotatedCommitPtr annotatedCommit;

        QString fromName;
        QString toName;

        ErrorResponse errorResponse;
};

Merge::Merge(RepositoryPtr repo, BranchPtr branch, QObject* parent) :
    GitOperation{repo, parent} {
    d = new MergePrivate();
    d->repo = repo;
    d->fromBranch = branch;
    d->annotatedCommit = d->fromBranch->toReference()->git_reference()->toAnnotatedCommit(repo->git_repository());
    d->errorResponse = ErrorResponse(ErrorResponse::UnspecifiedError, tr("Unspecified Error"));

    d->fromName = branch->name();
    d->toName = repo->head()->asBranch()->name();

    const git_annotated_commit* annotatedCommit = d->annotatedCommit->gitAnnotatedCommit();
    git_merge_analysis(&d->mergeAnalysis, &d->mergePreference, repo->git_repository()->gitRepository(), &annotatedCommit, 1);
}

Merge::~Merge() {
    delete d;
}

BranchPtr Merge::fromBranch() {
    return d->fromBranch;
}

Merge::MergeType Merge::mergeType() {
    if (d->repo->git_repository()->state() != LGRepository::IdleRepositoryState) {
        return MergeNotPossible;
    }

    if (!d->repo->head()->asBranch()) {
        return MergeNotPossible;
    }

    if (d->mergeAnalysis & GIT_MERGE_ANALYSIS_UP_TO_DATE) {
        return UpToDate;
    }

    if (d->mergeAnalysis & GIT_MERGE_ANALYSIS_UNBORN) {
        return FastForward;
    }

    if (d->mergeAnalysis & GIT_MERGE_ANALYSIS_FASTFORWARD && (d->mergePreference != GIT_MERGE_PREFERENCE_NO_FASTFORWARD)) {
        return FastForward;
    }

    if (d->mergeAnalysis & GIT_MERGE_ANALYSIS_NORMAL) {
        return MergeCommit;
    }

    return MergeNotPossible;
}

Merge::MergeResult Merge::performMerge() {
    switch (mergeType()) {
        case Merge::UpToDate:
        case Merge::MergeNotPossible:
            // Nothing to do!
            return MergeComplete;
        case Merge::FastForward:
            {
                ReferencePtr head = d->repo->head();
                if (!head) {
                    // TODO: Merge an unborn HEAD
                    d->errorResponse = ErrorResponse(ErrorResponse::UnspecifiedError, tr("Tried to merge an unborn HEAD"));
                    return MergeFailed;
                }

                if (CHK_ERR(d->repo->git_repository()->checkoutTree(d->fromBranch->toReference()->git_reference(), {}))) {
                    d->errorResponse = error;
                    return MergeFailed;
                }

                if (CHK_ERR(head->git_reference()->setTarget(d->annotatedCommit->oid(), "Merge"))) {
                    d->errorResponse = error;
                    return MergeFailed;
                }

                return MergeComplete;
            }
        case Merge::MergeCommit:
            {
                // Perform a merge commit
                git_merge_options merge_opts = GIT_MERGE_OPTIONS_INIT;
                git_checkout_options checkout_opts = GIT_CHECKOUT_OPTIONS_INIT;

                merge_opts.flags = 0;
                merge_opts.file_flags = GIT_MERGE_FILE_STYLE_DIFF3;

                checkout_opts.checkout_strategy = GIT_CHECKOUT_FORCE | GIT_CHECKOUT_ALLOW_CONFLICTS;

                const git_annotated_commit* annotatedCommit = d->annotatedCommit->gitAnnotatedCommit();
                if (git_merge(d->repo->git_repository()->gitRepository(), &annotatedCommit, 1, &merge_opts, &checkout_opts) != 0) {
                    d->errorResponse = ErrorResponse::fromCurrentGitError();
                    return MergeFailed;
                }

                LGIndexPtr index = d->repo->git_repository()->index();
                if (index->hasConflicts()) {
                    // Merging resulted in conflicts
                    return MergeConflict;
                }

                finaliseOperation();
                return MergeComplete;
            }
    }
}

ErrorResponse Merge::mergeFailureReason() {
    return d->errorResponse;
}

Merge::MergeNotPossibleReason Merge::mergeNotPossibleReason() {
    if (d->repo->git_repository()->state() != LGRepository::IdleRepositoryState) {
        return MergeNotPossibleBecauseRepositoryNotIdle;
    }

    if (!d->repo->head()->asBranch()) {
        return MergeNotPossibleBecauseHeadDetached;
    }

    return MergeNotPossibleUnknownReason;
}

RepositoryPtr Merge::repository() {
    return d->repo;
}

void Merge::finaliseOperation() {
    // Create the merge commit
    auto repo = d->repo->git_repository();
    auto sig = repo->defaultSignature();
    auto treeOid = repo->index()->writeTree(repo);
    auto tree = repo->lookupTree(treeOid);

    // Create a commit on the existing HEAD

    auto mergeMessage = tr("Merge %1 into %2").arg(d->fromName, d->toName);
    repo->createCommit(sig, sig, mergeMessage, tree, {d->repo->head()->asCommit()->gitCommit(), d->fromBranch->lastCommit()->gitCommit()});
}

PullMerge::PullMerge(RepositoryPtr repo, BranchPtr branch, QObject* parent) :
    Merge(repo, branch, parent) {
}
