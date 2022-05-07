#include "lgannotatedcommit.h"

#include "lgoid.h"
#include <git2.h>

struct LGAnnotatedCommitPrivate {
        git_annotated_commit* gitAnnotatedCommit;
};

LGAnnotatedCommit::LGAnnotatedCommit(struct git_annotated_commit* git_annotated_commit) :
    QObject{nullptr} {
    d = new LGAnnotatedCommitPrivate();
    d->gitAnnotatedCommit = git_annotated_commit;
}

LGAnnotatedCommit::~LGAnnotatedCommit() {
    git_annotated_commit_free(d->gitAnnotatedCommit);
    delete d;
}

git_annotated_commit* LGAnnotatedCommit::gitAnnotatedCommit() {
    return d->gitAnnotatedCommit;
}

LGOidPtr LGAnnotatedCommit::oid() {
    return LGOidPtr(new LGOid(*git_annotated_commit_id(d->gitAnnotatedCommit)));
}
