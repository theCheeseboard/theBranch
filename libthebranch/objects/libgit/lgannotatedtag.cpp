#include "lgannotatedtag.h"

#include "lgcommit.h"
#include "lgobject.h"
#include "lgrepository.h"
#include <git2.h>

struct LGAnnotatedTagPrivate {
        git_tag* gitTag;
};

LGAnnotatedTag::LGAnnotatedTag(git_tag* git_tag) {
    d = new LGAnnotatedTagPrivate();
    d->gitTag = git_tag;
}

LGAnnotatedTag::~LGAnnotatedTag() {
    if (d->gitTag) {
        git_tag_free(d->gitTag);
        delete d;
    }
}

git_tag* LGAnnotatedTag::gitTag() {
    return d->gitTag;
}

git_tag* LGAnnotatedTag::takeGitTag() {
    struct git_tag* ref = d->gitTag;
    d->gitTag = nullptr;
    return ref;
}

LGAnnotatedTagPtr LGAnnotatedTag::dup() {
    struct git_tag* tag;
    ::git_tag_dup(&tag, d->gitTag);
    return (new LGAnnotatedTag(tag))->tbSharedFromThis<LGAnnotatedTag>::sharedFromThis();
}

QString LGAnnotatedTag::name() {
    return QString::fromUtf8(git_tag_name(d->gitTag));
}

LGCommitPtr LGAnnotatedTag::target(LGRepositoryPtr repo) {
    git_object* target;
    if (git_tag_peel(&target, d->gitTag) != 0) return nullptr;

    LGObject object(target);
    if (object.type() != LGObject::Type::Commit) return nullptr;

    return repo->lookupCommit(object.oid());
}

bool LGAnnotatedTag::deleteTag(LGRepositoryPtr repo) {
    return git_tag_delete(repo->gitRepository(), git_tag_name(d->gitTag));
}
