#include "lgrevwalk.h"

#include "lgoid.h"
#include "lgrepository.h"
#include <git2.h>

struct LGRevwalkPrivate {
        git_revwalk* gitRevwalk;
};

LGRevwalk::LGRevwalk(struct git_revwalk* git_revwalk) :
    QObject{nullptr} {
    d = new LGRevwalkPrivate();
    d->gitRevwalk = git_revwalk;
}

LGRevwalk::~LGRevwalk() {
    git_revwalk_free(d->gitRevwalk);
    delete d;
}

LGRevwalkPtr LGRevwalk::revwalk_new(LGRepositoryPtr repo) {
    struct git_revwalk* revwalk;
    if (git_revwalk_new(&revwalk, repo->gitRepository()) != 0) return nullptr;
    return (new LGRevwalk(revwalk))->sharedFromThis();
}

void LGRevwalk::sorting(uint sortMode) {
    git_revwalk_sorting(d->gitRevwalk, sortMode);
}

void LGRevwalk::push(LGOidPtr oid) {
    git_revwalk_push(d->gitRevwalk, &oid->gitOid());
}

QList<LGOidPtr> LGRevwalk::walk() {
    QList<LGOidPtr> oids;
    git_oid oid;
    while (git_revwalk_next(&oid, d->gitRevwalk) == 0) {
        oids.append((new LGOid(oid))->sharedFromThis());
    }
    return oids;
}

git_revwalk* LGRevwalk::gitRevwalk() {
    return d->gitRevwalk;
}
