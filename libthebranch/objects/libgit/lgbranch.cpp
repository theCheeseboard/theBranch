#include "lgbranch.h"

#include <git2.h>

struct LGReferencePrivate {
        git_reference* gitReference;
};

LGBranch::LGBranch(struct git_reference* git_branch) :
    QObject{nullptr} {
    d = new LGReferencePrivate();
    d->gitReference = git_branch;
}

LGBranch::~LGBranch() {
    if (d->gitReference) {
        git_reference_free(d->gitReference);
        delete d;
    }
}

git_reference* LGBranch::gitReference() {
    return d->gitReference;
}

git_reference* LGBranch::takeGitReference() {
    struct git_reference* ref = d->gitReference;
    d->gitReference = nullptr;
    return ref;
}

LGBranchPtr LGBranch::dup() {
    struct git_reference* ref;
    ::git_reference_dup(&ref, d->gitReference);
    return LGBranchPtr(new LGBranch(ref));
}

QString LGBranch::name() {
    const char* name;
    if (git_branch_name(&name, d->gitReference) != 0) return "";
    return QString::fromUtf8(name);
}
