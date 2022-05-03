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
    git_reference_free(d->git_reference);
    delete d;
}

git_reference* LGBranch::git_reference() {
    return d->git_reference;
}

QString LGBranch::name() {
    const char* name;
    if (git_branch_name(&name, d->git_reference) != 0) return "";
    return QString::fromUtf8(name);
}
