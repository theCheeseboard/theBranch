#include "lgindex.h"

#include <git2.h>

struct LGIndexPrivate {
        git_index* gitIndex;
};

LGIndex::LGIndex(struct git_index* git_index) :
    QObject{nullptr} {
    d = new LGIndexPrivate();
    d->gitIndex = git_index;
}

LGIndex::LGIndex() {
    d = new LGIndexPrivate();
    git_index_new(&d->gitIndex);
}

LGIndex::~LGIndex() {
    git_index_free(d->gitIndex);
    delete d;
}

git_index* LGIndex::gitIndex() {
    return d->gitIndex;
}

bool LGIndex::hasConflicts() {
    return git_index_has_conflicts(d->gitIndex);
}

void LGIndex::conflictCleanup() {
    git_index_conflict_cleanup(d->gitIndex);
}

void LGIndex::addAll(QStringList globs) {
    git_strarray strarray;
    strarray.strings = new char*[globs.count()];
    strarray.count = globs.count();
    for (int i = 0; i < globs.count(); i++) {
        strarray.strings[i] = globs.at(i).toUtf8().data();
    }
    git_index_add_all(d->gitIndex, &strarray, 0, nullptr, nullptr);
    delete[] strarray.strings;
}
