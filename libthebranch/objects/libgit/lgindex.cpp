#include "lgindex.h"

#include "lgoid.h"
#include "lgrepository.h"
#include "lgtree.h"
#include <QFileInfo>
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

bool LGIndex::write() {
    return git_index_write(d->gitIndex) == 0;
}

bool LGIndex::hasConflicts() {
    return git_index_has_conflicts(d->gitIndex);
}

void LGIndex::conflictCleanup() {
    git_index_conflict_cleanup(d->gitIndex);
}

bool LGIndex::readTree(LGTreePtr tree) {
    return git_index_read_tree(d->gitIndex, tree->gitTree()) == 0;
}

LGOidPtr LGIndex::writeTree(LGRepositoryPtr repo) {
    git_oid oid;
    if (git_index_write_tree_to(&oid, d->gitIndex, repo->gitRepository()) != 0) return nullptr;
    return (new LGOid(oid))->sharedFromThis();
}

bool LGIndex::addByPath(QString path) {
    return git_index_add_bypath(d->gitIndex, path.toUtf8().data()) == 0;
}

bool LGIndex::addAll(QStringList globs) {
    git_strarray strarray;
    strarray.strings = new char*[globs.count()];
    strarray.count = globs.count();
    for (int i = 0; i < globs.count(); i++) {
        strarray.strings[i] = globs.at(i).toUtf8().data();
    }
    if (git_index_add_all(d->gitIndex, &strarray, 0, nullptr, nullptr) != 0) {
        delete[] strarray.strings;
        return false;
    }
    delete[] strarray.strings;
    return true;
}

bool LGIndex::addBuffer(QFileInfo fileInfo, QString pathspec, QByteArray data) {
    git_index_entry indexEntry;
    indexEntry.path = pathspec.toUtf8().data();
    indexEntry.ctime.seconds = fileInfo.birthTime().toSecsSinceEpoch();
    indexEntry.ctime.nanoseconds = fileInfo.birthTime().toMSecsSinceEpoch() % 1000 * 1000;
    indexEntry.mtime.seconds = fileInfo.lastModified().toMSecsSinceEpoch() % 1000 * 1000;
    indexEntry.mtime.nanoseconds = fileInfo.lastModified().toMSecsSinceEpoch() % 1000 * 1000;
    indexEntry.uid = fileInfo.ownerId();
    indexEntry.gid = fileInfo.groupId();

    if (fileInfo.isSymLink()) {
        indexEntry.mode = 0120000;
        data = fileInfo.symLinkTarget().toUtf8();
    } else {
        indexEntry.file_size = data.length();
        if (fileInfo.permission(QFile::ExeUser)) {
            indexEntry.mode = 0100755;
        } else {
            indexEntry.mode = 0100644;
        }
    }

    return git_index_add_from_buffer(d->gitIndex, &indexEntry, data.data(), data.length()) == 0;
}

bool LGIndex::removeByPath(QString path) {
    return git_index_remove_bypath(d->gitIndex, path.toUtf8().data()) == 0;
}

bool LGIndex::removeAll(QStringList globs) {
    git_strarray strarray;
    strarray.strings = new char*[globs.count()];
    strarray.count = globs.count();
    for (int i = 0; i < globs.count(); i++) {
        strarray.strings[i] = globs.at(i).toUtf8().data();
    }
    if (git_index_remove_all(d->gitIndex, &strarray, nullptr, nullptr) != 0) {
        delete[] strarray.strings;
        return false;
    }
    delete[] strarray.strings;
    return true;
}
