#include "lgtag.h"

#include "lgrepository.h"
#include <git2.h>

struct LGTagPrivate {
        git_tag* gitTag;
};

LGTag::LGTag(git_tag* git_tag) {
    d = new LGTagPrivate();
    d->gitTag = git_tag;
}

LGTag::~LGTag() {
    if (d->gitTag) {
        git_tag_free(d->gitTag);
        delete d;
    }
}

git_tag* LGTag::gitTag() {
    return d->gitTag;
}

git_tag* LGTag::takeGitTag() {
    struct git_tag* ref = d->gitTag;
    d->gitTag = nullptr;
    return ref;
}

LGTagPtr LGTag::dup() {
    struct git_tag* tag;
    ::git_tag_dup(&tag, d->gitTag);
    return (new LGTag(tag))->sharedFromThis();
}

QString LGTag::name() {
    return QString::fromUtf8(git_tag_name(d->gitTag));
}

bool LGTag::deleteTag(LGRepositoryPtr repo) {
    return git_tag_delete(repo->gitRepository(), git_tag_name(d->gitTag));
}
