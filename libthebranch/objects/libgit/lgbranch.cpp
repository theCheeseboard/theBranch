#include "lgbranch.h"

#include "lgrepository.h"
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

bool LGBranch::isRemoteBranch() {
    return git_reference_is_remote(d->gitReference);
}

QString LGBranch::localBranchName(LGRepositoryPtr repo) {
    git_buf buf = GIT_BUF_INIT;
    git_branch_remote_name(&buf, repo->gitRepository(), ("refs/remotes/" + this->name()).toUtf8().data());
    QString remoteName = QString::fromUtf8(buf.ptr, buf.size);

    QString branchName = this->name().remove(remoteName);
    if (branchName.startsWith("/")) branchName = branchName.mid(1);

    return branchName;
}

LGBranchPtr LGBranch::upstream() {
    git_reference* upstreamBranch;
    if (git_branch_upstream(&upstreamBranch, d->gitReference) != 0) return LGBranchPtr();
    return LGBranchPtr(new LGBranch(upstreamBranch));
}

bool LGBranch::setUpstream(LGBranchPtr upstream) {
    if (upstream) {
        return git_branch_set_upstream(d->gitReference, nullptr) == 0;
    } else {
        return git_branch_set_upstream(d->gitReference, (upstream->name()).toUtf8().data()) == 0;
    }
}

bool LGBranch::deleteBranch() {
    return git_branch_delete(d->gitReference) == 0;
}
