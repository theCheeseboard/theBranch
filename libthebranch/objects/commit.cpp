#include "commit.h"

#include "libgit/lgcommit.h"
#include "libgit/lgoid.h"
#include "libgit/lgsignature.h"
#include "tree.h"

struct CommitPrivate {
        LGCommitPtr commit;
        LGRepositoryPtr repo;
        QList<CommitPtr> commits;
};

Commit::~Commit() {
    delete d;
}

bool Commit::equal(CommitPtr other) {
    return d->commit->oid()->equal(other->d->commit->oid());
}

QString Commit::commitMessage() {
    return d->commit->message();
}

QString Commit::commitHash() {
    return d->commit->oid()->toHex();
}

QString Commit::shortCommitHash() {
    // HACK: We should go through the repo and find the shortest string to uniquely identify a commit
    return d->commit->oid()->toHex().left(7);
}

QString Commit::authorName() {
    return d->commit->committer()->name();
}

TreePtr Commit::tree() {
    return Tree::treeForLgTree(d->repo, d->commit->tree());
}

QList<CommitPtr> Commit::parents() {
    QList<CommitPtr> parents;
    for (auto i = 0; i < d->commit->parentCount(); i++) {
        parents.append(Commit::commitForLgCommit(d->repo, d->commit->parent(i)));
    }
    return parents;
}

CommitPtr Commit::commitForLgCommit(LGRepositoryPtr repo, LGCommitPtr commit) {
    auto* c = new Commit();
    c->d->commit = commit;
    c->d->repo = repo;
    return c->sharedFromThis();
}

LGCommitPtr Commit::gitCommit() {
    return d->commit;
}

Commit::Commit(QObject* parent) :
    QObject{parent} {
    d = new CommitPrivate;
}
