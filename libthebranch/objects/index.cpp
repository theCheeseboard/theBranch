#include "index.h"

#include "libgit/lgindex.h"
#include "libgit/lgrepository.h"
#include "repository.h"
#include <git2.h>

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

bool Index::hasChangesFromWorkdir(RepositoryPtr repo) {
    git_diff* diff;
    if (git_diff_index_to_workdir(&diff, repo->git_repository()->gitRepository(), d->index->gitIndex(), nullptr) != 0) {
        return true;
    }

    git_diff_stats* stats;
    git_diff_get_stats(&stats, diff);

    auto changedFiles = git_diff_stats_files_changed(stats);

    git_diff_stats_free(stats);
    git_diff_free(diff);

    return changedFiles > 0;
}

QList<Index::IndexItem> Index::items() {
    QList<IndexItem> items;

    git_index_iterator* iterator;
    if (git_index_iterator_new(&iterator, d->index->gitIndex()) != 0) return {};

    const git_index_entry* indexEntry;
    while (!git_index_iterator_next(&indexEntry, iterator)) {
        IndexItem item;

        items.append(item);
    }

    git_index_iterator_free(iterator);

    return items;
}

IndexPtr Index::indexForLgIndex(LGIndexPtr index) {
    Index* i = new Index();
    i->d->index = index;
    return i->sharedFromThis();
}
