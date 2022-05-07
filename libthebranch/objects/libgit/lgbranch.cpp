#include "lgbranch.h"

#include <git2.h>

struct LGReferencePrivate {
        git_reference* git_reference;
};

LGBranch::LGBranch(struct git_reference* git_branch) :
    QObject{nullptr} {
    d = new LGReferencePrivate();
    d->git_reference = git_branch;
}

LGBranch::~LGBranch() {
    if (d->git_reference) {
        git_reference_free(d->git_reference);
        delete d;
    }
}

git_reference* LGBranch::git_reference() {
    return d->git_reference;
}

git_reference* LGBranch::take_git_reference() {
    struct git_reference* ref = d->git_reference;
    d->git_reference = nullptr;
    return ref;
}

LGBranchPtr LGBranch::dup() {
    struct git_reference* ref;
    ::git_reference_dup(&ref, d->git_reference);
    return LGBranchPtr(new LGBranch(ref));
}

QString LGBranch::name() {
    const char* name;
    if (git_branch_name(&name, d->git_reference) != 0) return "";
    return QString::fromUtf8(name);
}
