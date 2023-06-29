#include "tag.h"

#include "commit.h"
#include "libgit/lgrepository.h"
#include "libgit/lgtag.h"
#include <git2.h>

struct TagPrivate {
        LGTagPtr tag;
        LGRepositoryPtr repo;
};

Tag::~Tag() {
    delete d;
}

QString Tag::name() {
    return d->tag->name();
}

ErrorResponse Tag::deleteTag() {
    if (!d->tag->deleteTag(d->repo)) {
        return ErrorResponse::fromCurrentGitError();
    }
    return ErrorResponse();
}

TagPtr Tag::tagForLgTag(LGRepositoryPtr repo, LGTagPtr tag) {
    auto* t = new Tag();
    t->d->tag = tag;
    t->d->repo = repo;
    return t->sharedFromThis();
}

LGTagPtr Tag::gitTag() {
    return d->tag;
}

Tag::Tag(QObject* parent) :
    QObject{parent} {
    d = new TagPrivate();
}

CommitPtr Tag::resolveToCommit() {
    return Commit::commitForLgCommit(d->repo, d->tag->target(d->repo));
}
