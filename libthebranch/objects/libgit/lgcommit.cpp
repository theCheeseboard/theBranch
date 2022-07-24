#include "lgcommit.h"

#include "lgoid.h"
#include "lgrepository.h"
#include "lgsignature.h"
#include "lgtree.h"
#include <git2.h>

struct LGCommitPrivate {
        ::git_commit* git_commit;
};

LGCommit::LGCommit(struct git_commit* git_commit) :
    QObject{nullptr} {
    d = new LGCommitPrivate();
    d->git_commit = git_commit;
}

LGCommit::~LGCommit() {
    git_commit_free(d->git_commit);
    delete d;
}

LGCommitPtr LGCommit::lookup(LGRepositoryPtr repo, LGOidPtr oid) {
    struct git_commit* commit;
    if (git_commit_lookup(&commit, repo->gitRepository(), &oid->gitOid()) != 0) return nullptr;
    return (new LGCommit(commit))->sharedFromThis();
}

git_commit* LGCommit::gitCommit() {
    return d->git_commit;
}

QString LGCommit::message() {
    return QString::fromUtf8(git_commit_message(d->git_commit));
}

LGSignaturePtr LGCommit::committer() {
    return (new LGSignature(git_commit_committer(d->git_commit)))->sharedFromThis();
}

LGOidPtr LGCommit::oid() {
    const git_oid* oid = git_commit_id(d->git_commit);
    return (new LGOid(*oid))->sharedFromThis();
}

LGTreePtr LGCommit::tree() {
    git_tree* tree;
    if (git_commit_tree(&tree, d->git_commit) != 0) return nullptr;
    return (new LGTree(tree))->sharedFromThis();
}

int LGCommit::parentCount() {
    return git_commit_parentcount(d->git_commit);
}

LGCommitPtr LGCommit::parent(int n) {
    git_commit* parent;
    if (git_commit_parent(&parent, d->git_commit, n) != 0) return nullptr;
    return (new LGCommit(parent))->sharedFromThis();
}
