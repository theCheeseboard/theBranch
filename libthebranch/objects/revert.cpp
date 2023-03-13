#include "revert.h"

#include "commit.h"
#include "index.h"
#include "libgit/lgcommit.h"
#include "libgit/lgindex.h"
#include "libgit/lgrepository.h"
#include "reference.h"
#include "repository.h"
#include <git2.h>

struct RevertPrivate {
        RepositoryPtr repo;
        CommitPtr commit;
        int mainlineCommit = -1;

        ErrorResponse error;
};

Revert::Revert(RepositoryPtr repo, CommitPtr commit, QObject* parent) :
    GitOperation{repo, parent} {
    d = new RevertPrivate();
    d->repo = repo;
    d->commit = commit;
}

Revert::~Revert() {
    delete d;
}

CommitPtr Revert::commit() {
    return d->commit;
}

void Revert::setMainlineCommit(CommitPtr mainlineCommit) {
    auto parents = d->commit->parents();
    for (auto i = 0; i < parents.length(); i++) {
        if (parents.at(i)->equal(mainlineCommit)) {
            d->mainlineCommit = i + 1;
            return;
        }
    }
    d->mainlineCommit = -1;
}

Revert::RevertResult Revert::performRevert() {
    git_revert_options options = GIT_REVERT_OPTIONS_INIT;
    if (d->mainlineCommit != -1) options.mainline = d->mainlineCommit;
    if (git_revert(d->repo->git_repository()->gitRepository(), d->commit->gitCommit()->gitCommit(), &options) != 0) {
        d->error = ErrorResponse::fromCurrentGitError();
        return RevertFailed;
    }

    if (d->repo->index()->hasConflicts()) {
        return RevertConflict;
    }

    if (!d->repo->index()->hasChangesFromWorkdir(d->repo)) {
        this->abortOperation();
        d->error = ErrorResponse(ErrorResponse::UnspecifiedError, tr("The result of the revert was empty"));
        return RevertFailed;
    }

    this->finaliseOperation();
    return RevertComplete;
}

ErrorResponse Revert::revertFailureReason() {
    return d->error;
}

void Revert::finaliseOperation() {
    // Create the commit
    auto repo = d->repo->git_repository();
    auto sig = repo->defaultSignature();
    auto head = d->repo->head();
    auto treeOid = repo->index()->writeTree(repo);
    auto tree = repo->lookupTree(treeOid);

    // Create a commit on the existing HEAD
    auto commitMessage = tr("Revert %1\n\nThis commit reverts %2").arg(QLocale().quoteString(d->commit->commitMessage()), d->commit->commitHash());
    repo->createCommit(sig, sig, commitMessage, tree, {head->asCommit()->gitCommit()});
}
