#include "lgrevwalk.h"

#include "lgoid.h"
#include "lgrepository.h"
#include <git2.h>

struct LGRevwalkPrivate {
        git_revwalk* git_revwalk;
};

LGRevwalk::LGRevwalk(struct git_revwalk* git_revwalk) :
    QObject{nullptr} {
    d = new LGRevwalkPrivate();
    d->git_revwalk = git_revwalk;
}

LGRevwalk::~LGRevwalk() {
    git_revwalk_free(d->git_revwalk);
    delete d;
}

LGRevwalkPtr LGRevwalk::revwalk_new(LGRepositoryPtr repo) {
    struct git_revwalk* revwalk;
    if (git_revwalk_new(&revwalk, repo->git_repository()) != 0) return nullptr;
    return LGRevwalkPtr(new LGRevwalk(revwalk));
}

void LGRevwalk::sorting(uint sortMode) {
    git_revwalk_sorting(d->git_revwalk, sortMode);
}

void LGRevwalk::push(LGOidPtr oid) {
    git_revwalk_push(d->git_revwalk, &oid->git_oid());
}

QList<LGOidPtr> LGRevwalk::walk() {
    QList<LGOidPtr> oids;
    git_oid oid;
    while (git_revwalk_next(&oid, d->git_revwalk) == 0) {
        oids.append(LGOidPtr(new LGOid(oid)));
    }
    return oids;
}

git_revwalk* LGRevwalk::git_revwalk() {
    return d->git_revwalk;
}
