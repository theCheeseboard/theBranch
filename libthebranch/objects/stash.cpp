#include "stash.h"

#include "libgit/lgrepository.h"
#include "libgit/lgstash.h"
#include <git2.h>

struct StashPrivate {
        LGRepositoryPtr repo;
        LGStashPtr stash;
};

Stash::Stash(QObject* parent) :
    QObject{parent} {
    d = new StashPrivate();
}

StashPtr Stash::stashForLgStash(LGStashPtr stash, LGRepositoryPtr repo) {
    auto* s = new Stash();
    s->d->stash = stash;
    s->d->repo = repo;
    return s->sharedFromThis();
}

Stash::~Stash() {
    delete d;
}

QString Stash::message() {
    return d->stash->message();
}

bool Stash::apply() {
    git_stash_apply_options options = GIT_STASH_APPLY_OPTIONS_INIT;
    return git_stash_apply(d->repo->gitRepository(), d->stash->index(), &options) == 0;
}

void Stash::drop() {
    git_stash_drop(d->repo->gitRepository(), d->stash->index());
}
