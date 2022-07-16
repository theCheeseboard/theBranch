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

QString Commit::commitMessage() {
    return d->commit->message();
}

QString Commit::commitHash() {
    return d->commit->oid()->toHex();
}

QString Commit::authorName() {
    return d->commit->committer()->name();
}

CommitPtr Commit::commitForLgCommit(LGCommitPtr commit) {
    Commit* c = new Commit();
    c->d->commit = commit;
    return c->sharedFromThis();
}

Commit::Commit(QObject* parent) :
    QObject{parent} {
    d = new CommitPrivate;
}
