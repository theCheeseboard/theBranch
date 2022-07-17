#include "reference.h"

#include "branch.h"
#include "libgit/lgbranch.h"
#include "libgit/lgreference.h"

struct ReferencePrivate {
        LGReferencePtr reference;
        LGRepositoryPtr repo;
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
        return Branch::branchForLgBranch(d->repo, (new LGBranch(d->reference->dup()->takeGitReference()))->sharedFromThis());
    }
    return nullptr;
}

LGReferencePtr Reference::git_reference() {
    return d->reference;
}

QString Reference::name() {
    return d->reference->name();
}

QString Reference::shorthand() {
    return d->reference->shorthand();
}

QString Reference::symbolicTarget() {
    return d->reference->symbolicTarget();
}

ReferencePtr Reference::referenceForLgReference(LGRepositoryPtr repo, LGReferencePtr reference) {
    Reference* r = new Reference();
    r->d->reference = reference;
    r->d->repo = repo;
    return r->sharedFromThis();
}
