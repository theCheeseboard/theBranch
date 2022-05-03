#include "reference.h"

#include "branch.h"
#include "libgit/lgbranch.h"
#include "libgit/lgreference.h"

struct ReferencePrivate {
        LGReferencePtr reference;
};

Reference::Reference(QObject* parent) :
    QObject{parent} {
    d = new ReferencePrivate;
}

Reference::~Reference() {
    delete d;
}

BranchPtr Reference::asBranch() {
    if (d->reference->isBranch()) {
        return Branch::branchForLgBranch(LGBranchPtr(new LGBranch(d->reference->git_reference())));
    }
    return nullptr;
}

QString Reference::name() {
    return d->reference->name();
}

QString Reference::shorthand() {
    return d->reference->shorthand();
}

ReferencePtr Reference::referenceForLgReference(LGReferencePtr reference) {
    Reference* r = new Reference();
    r->d->reference = reference;
    return ReferencePtr(r);
}
