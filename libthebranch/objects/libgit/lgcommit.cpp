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
    return LGCommitPtr(new LGCommit(commit));
}

git_commit* LGCommit::gitCommit() {
    return d->git_commit;
}

QString LGCommit::message() {
    return QString::fromUtf8(git_commit_message(d->git_commit));
}

LGSignaturePtr LGCommit::committer() {
    return LGSignaturePtr(new LGSignature(git_commit_committer(d->git_commit)));
}

LGOidPtr LGCommit::oid() {
    const git_oid* oid = git_commit_id(d->git_commit);
    return LGOidPtr(new LGOid(*oid));
}

LGTreePtr LGCommit::tree() {
    git_tree* tree;
    if (git_commit_tree(&tree, d->git_commit) != 0) return LGTreePtr();
    return LGTreePtr(new LGTree(tree));
}
