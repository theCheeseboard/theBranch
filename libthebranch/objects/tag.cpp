#include "tag.h"

#include "commit.h"
#include "libgit/lgobject.h"
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
    git_object* target;
    if (git_tag_peel(&target, d->tag->gitTag()) != 0) return nullptr;

    LGObject object(target);
    if (object.type() != LGObject::Type::Commit) return nullptr;

    return Commit::commitForLgCommit(d->repo, d->repo->lookupCommit(object.oid()));
}
