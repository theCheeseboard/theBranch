#include "commit.h"

#include "libgit/lgcommit.h"
#include "libgit/lgoid.h"
#include "libgit/lgsignature.h"

struct CommitPrivate {
        LGCommitPtr commit;
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

QString Commit::authorName() {
    return d->commit->committer()->name();
}

QList<CommitPtr> Commit::parents() {
    QList<CommitPtr> parents;
    for (auto i = 0; i < d->commit->parentCount(); i++) {
        parents.append(Commit::commitForLgCommit(d->commit->parent(i)));
    }
    return parents;
}

CommitPtr Commit::commitForLgCommit(LGCommitPtr commit) {
    Commit* c = new Commit();
    c->d->commit = commit;
    return c->sharedFromThis();
}

LGCommitPtr Commit::gitCommit() {
    return d->commit;
}

Commit::Commit(QObject* parent) :
    QObject{parent} {
    d = new CommitPrivate;
}
