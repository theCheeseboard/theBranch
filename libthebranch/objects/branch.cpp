#include "branch.h"

#include "commit.h"
#include "libgit/lgbranch.h"
#include "libgit/lgcommit.h"
#include "libgit/lgreference.h"
#include "libgit/lgrepository.h"
#include "reference.h"

#include <git2.h>

struct BranchPrivate {
        LGBranchPtr branch;
        LGRepositoryPtr repo;
};

Branch::~Branch() {
    delete d;
}

bool Branch::equal(ReferencePtr ref) {
    return ref->git_reference()->isEqual(this->toReference()->git_reference());
}

bool Branch::equal(BranchPtr branch) {
    return branch->name() == this->name() && branch->toReference()->shorthand() == this->toReference()->shorthand();
}

ReferencePtr Branch::toReference() {
    return Reference::referenceForLgReference(d->repo, (new LGReference(d->branch->dup()->takeGitReference()))->sharedFromThis());
}

CommitPtr Branch::lastCommit() {
    auto ref = this->toReference()->git_reference();
    auto oid = ref->resolve()->target();
    return Commit::commitForLgCommit(LGCommit::lookup(d->repo, oid));
}

QString Branch::name() {
    return d->branch->name();
}

bool Branch::isRemoteBranch() {
    return d->branch->isRemoteBranch();
}

QString Branch::remoteName() {
    return d->branch->remoteName(d->repo);
}

QString Branch::localBranchName() {
    return d->branch->localBranchName(d->repo);
}

ErrorResponse Branch::deleteBranch() {
    if (!d->branch->deleteBranch()) {
        return ErrorResponse::fromCurrentGitError();
    }
    return ErrorResponse();
}

QCoro::Task<> Branch::deleteRemoteBranch() {
    QStringList args = {
        "push",
        "-d",
        this->remoteName(),
        this->localBranchName()};
    auto [exitCode, gitError] = co_await d->repo->runGit(args);
}

BranchPtr Branch::upstream() {
    auto upstream = d->branch->upstream();
    if (!upstream) return nullptr;
    return Branch::branchForLgBranch(d->repo, upstream);
}

BranchPtr Branch::branchForLgBranch(LGRepositoryPtr repo, LGBranchPtr branch) {
    auto* b = new Branch();
    b->d->branch = branch;
    b->d->repo = repo;
    return b->sharedFromThis();
}

LGBranchPtr Branch::gitBranch() {
    return d->branch;
}

Branch::Branch(QObject* parent) :
    QObject{parent} {
    d = new BranchPrivate;
}
