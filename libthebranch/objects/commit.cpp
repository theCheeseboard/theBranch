#include "commit.h"

#include "libgit/lgcommit.h"
#include "libgit/lgoid.h"
#include "libgit/lgrepository.h"
#include "libgit/lgrevwalk.h"
#include "libgit/lgsignature.h"
#include "tree.h"
#include <QDateTime>
#include <git2.h>

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

QDateTime Commit::date() {
    return d->commit->date();
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

QList<CommitPtr> Commit::history() {
    LGRevwalkPtr revwalk = LGRevwalk::revwalk_new(d->repo);
    revwalk->sorting(GIT_SORT_TOPOLOGICAL | GIT_SORT_TIME);
    revwalk->push(this->gitCommit()->oid());

    QList<CommitPtr> commits;
    QList<LGOidPtr> oids = revwalk->walk();
    for (const LGOidPtr& oid : oids) {
        LGCommitPtr commit = LGCommit::lookup(d->repo, oid);
        commits.append(Commit::commitForLgCommit(d->repo, commit)->sharedFromThis());
    }
    return commits;
}

CommitPtr Commit::lastCommonAncestor(CommitPtr commit) {
    CommitPtr lastCommonAncestor;
    QList<CommitPtr> ancestor1 = this->history();
    QList<CommitPtr> ancestor2 = commit->history();
    while (!ancestor1.isEmpty() && !ancestor2.isEmpty()) {
        auto commit1 = ancestor1.takeLast();
        auto commit2 = ancestor2.takeLast();

        if (!commit1->equal(commit2)) break;
        lastCommonAncestor = commit1;
    }
    return lastCommonAncestor;
}

int Commit::commitsUntil(CommitPtr commit) {
    int i = 0;
    for (auto history : this->history()) {
        if (history->equal(commit)) break;
        i++;
    }
    return i;
}

int Commit::missingCommits(CommitPtr commit) {
    auto otherHistory = commit->history();
    int i = 0;
    for (auto thisCommit : this->history()) {
        bool isOnOther = false;
        for (auto otherCommit : otherHistory) {
            if (thisCommit->equal(otherCommit)) {
                isOnOther = true;
                break;
            }
        }

        if (!isOnOther) i++;
    }
    return i;
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
