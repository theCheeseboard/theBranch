#include "branch.h"

#include "libgit/lgbranch.h"
#include "libgit/lgreference.h"
#include "reference.h"

#include <git2.h>

struct BranchPrivate {
        LGBranchPtr branch;
};

Branch::~Branch() {
    delete d;
}

ReferencePtr Branch::toReference() {
    return Reference::referenceForLgReference(LGReferencePtr(new LGReference(d->branch->dup()->take_git_reference())));
}

QString Branch::name() {
    return d->branch->name();
}

BranchPtr Branch::branchForLgBranch(LGBranchPtr branch) {
    Branch* b = new Branch();
    b->d->branch = branch;
    return BranchPtr(b);
}

Branch::Branch(QObject* parent) :
    QObject{parent} {
    d = new BranchPrivate;
}
