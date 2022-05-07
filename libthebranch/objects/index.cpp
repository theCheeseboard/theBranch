#include "index.h"

#include "libgit/lgindex.h"

struct IndexPrivate {
        LGIndexPtr index;
};

Index::Index(QObject* parent) :
    QObject{parent} {
    d = new IndexPrivate();
}

Index::~Index() {
    delete d;
}

bool Index::hasConflicts() {
    return d->index->hasConflicts();
}

void Index::conflictCleanup() {
    d->index->conflictCleanup();
}

IndexPtr Index::indexForLgIndex(LGIndexPtr index) {
    Index* i = new Index();
    i->d->index = index;
    return IndexPtr(i);
}
