#include "branch.h"

#include "libgit/lgbranch.h"
#include "libgit/lgreference.h"
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
    return equal(branch->toReference());
}

ReferencePtr Branch::toReference() {
    return Reference::referenceForLgReference(d->repo, LGReferencePtr(new LGReference(d->branch->dup()->takeGitReference())));
}

QString Branch::name() {
    return d->branch->name();
}

bool Branch::isRemoteBranch() {
    return d->branch->isRemoteBranch();
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

BranchPtr Branch::branchForLgBranch(LGRepositoryPtr repo, LGBranchPtr branch) {
    Branch* b = new Branch();
    b->d->branch = branch;
    b->d->repo = repo;
    return BranchPtr(b);
}

LGBranchPtr Branch::gitBranch() {
    return d->branch;
}

Branch::Branch(QObject* parent) :
    QObject{parent} {
    d = new BranchPrivate;
}
