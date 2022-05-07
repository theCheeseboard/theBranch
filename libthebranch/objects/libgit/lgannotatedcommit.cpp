#include "lgannotatedcommit.h"

#include "lgoid.h"
#include <git2.h>

struct LGAnnotatedCommitPrivate {
        git_annotated_commit* git_annotated_commit;
};

LGAnnotatedCommit::LGAnnotatedCommit(struct git_annotated_commit* git_annotated_commit) :
    QObject{nullptr} {
    d = new LGAnnotatedCommitPrivate();
    d->git_annotated_commit = git_annotated_commit;
}

LGAnnotatedCommit::~LGAnnotatedCommit() {
    git_annotated_commit_free(d->git_annotated_commit);
    delete d;
}

git_annotated_commit* LGAnnotatedCommit::git_annotated_commit() {
    return d->git_annotated_commit;
}

LGOidPtr LGAnnotatedCommit::oid() {
    return LGOidPtr(new LGOid(*git_annotated_commit_id(d->git_annotated_commit)));
}
