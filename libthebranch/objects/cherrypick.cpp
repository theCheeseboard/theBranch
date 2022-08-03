#include "cherrypick.h"

#include "commit.h"
#include "index.h"
#include "libgit/lgcommit.h"
#include "libgit/lgrepository.h"
#include "objects/libgit/lgindex.h"
#include "reference.h"
#include "repository.h"
#include <git2.h>

struct CherryPickPrivate {
        RepositoryPtr repo;
        CommitPtr commit;
        int mainlineCommit = -1;

        ErrorResponse error;
};

CherryPick::CherryPick(RepositoryPtr repo, CommitPtr commit, QObject* parent) :
    GitOperation{parent} {
    d = new CherryPickPrivate();
    d->repo = repo;
    d->commit = commit;
}

CherryPick::~CherryPick() {
    delete d;
}

CommitPtr CherryPick::commit() {
    return d->commit;
}

void CherryPick::setMainlineCommit(CommitPtr mainlineCommit) {
    auto parents = d->commit->parents();
    for (auto i = 0; i < parents.length(); i++) {
        if (parents.at(i)->equal(mainlineCommit)) {
            d->mainlineCommit = i + 1;
            return;
        }
    }
    d->mainlineCommit = -1;
}

CherryPick::CherryPickResult CherryPick::performCherryPick() {
    git_cherrypick_options options = GIT_CHERRYPICK_OPTIONS_INIT;
    if (d->mainlineCommit != -1) options.mainline = d->mainlineCommit;
    if (git_cherrypick(d->repo->git_repository()->gitRepository(), d->commit->gitCommit()->gitCommit(), &options) != 0) {
        d->error = ErrorResponse::fromCurrentGitError();
        return CherryPickFailed;
    }

    if (d->repo->index()->hasConflicts()) {
        return CherryPickConflict;
    }

    if (!d->repo->index()->hasChangesFromWorkdir(d->repo)) {
        this->abortOperation();
        d->error = ErrorResponse(ErrorResponse::UnspecifiedError, tr("The result of the cherry pick was empty"));
        return CherryPickFailed;
    }

    this->finaliseOperation();
    return CherryPickComplete;
}

ErrorResponse CherryPick::cherryPickFailureReason() {
    return d->error;
}

RepositoryPtr CherryPick::repository() {
    return d->repo;
}

void CherryPick::abortOperation() {
    // Abort the ongoing cherry pick
    d->repo->git_repository()->cleanupState();
    d->repo->git_repository()->checkoutTree(d->repo->git_repository()->head(), {
                                                                                   {"force", true}
    });
}

void CherryPick::finaliseOperation() {
    // Create the commit
    auto repo = d->repo->git_repository();
    auto sig = repo->defaultSignature();
    auto head = d->repo->head();
    auto treeOid = repo->index()->writeTree(repo);
    auto tree = repo->lookupTree(treeOid);

    // Create a commit on the existing HEAD
    auto commitMessage = tr("Cherry pick %1 into %2").arg(d->commit->commitHash(), head->asBranch()->name());
    repo->createCommit(sig, sig, commitMessage, tree, {head->asCommit()->gitCommit()});
}
