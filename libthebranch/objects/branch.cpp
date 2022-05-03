#include "branch.h"

#include "libgit/lgbranch.h"

struct BranchPrivate {
        LGBranchPtr branch;
};

Branch::~Branch() {
    delete d;
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
