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

LGIndex::LGIndex() {
    d = new LGIndexPrivate();
    git_index_new(&d->git_index);
}

LGIndex::~LGIndex() {
    git_index_free(d->git_index);
    delete d;
}

git_index* LGIndex::git_index() {
    return d->git_index;
}

bool LGIndex::hasConflicts() {
    return git_index_has_conflicts(d->git_index);
}

void LGIndex::conflictCleanup() {
    git_index_conflict_cleanup(d->git_index);
}

void LGIndex::addAll(QStringList globs) {
    git_strarray strarray;
    strarray.strings = new char*[globs.count()];
    strarray.count = globs.count();
    for (int i = 0; i < globs.count(); i++) {
        strarray.strings[i] = globs.at(i).toUtf8().data();
    }
    git_index_add_all(d->git_index, &strarray, 0, nullptr, nullptr);
    delete[] strarray.strings;
}
