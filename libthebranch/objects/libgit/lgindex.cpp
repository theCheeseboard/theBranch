#include "lgindex.h"

#include <git2.h>

struct LGIndexPrivate {
        git_index* git_index;
};

LGIndex::LGIndex(struct git_index* git_index) :
    QObject{nullptr} {
    d = new LGIndexPrivate();
    d->git_index = git_index;
}

LGIndex::~LGIndex() {
    git_index_free(d->git_index);
    delete d;
}

git_index* LGIndex::git_index() {
    return d->git_index;
}
