#include "lgreference.h"

#include <git2.h>

#include "lgoid.h"
#include "lgrepository.h"

struct LGReferencePrivate {
        git_reference* git_reference;
};

LGReference::LGReference(struct git_reference* git_reference) :
    QObject{nullptr} {
    d = new LGReferencePrivate();
    d->git_reference = git_reference;
}

LGReference::~LGReference() {
    git_reference_free(d->git_reference);
    delete d;
}

git_reference* LGReference::git_reference() {
    return d->git_reference;
}

bool LGReference::isBranch() {
    return git_reference_is_branch(d->git_reference);
}

QString LGReference::name() {
    return QString::fromUtf8(git_reference_name(d->git_reference));
}

QString LGReference::shorthand() {
    return QString::fromUtf8(git_reference_shorthand(d->git_reference));
}

LGReferencePtr LGReference::resolve() {
    struct git_reference* ref;
    git_reference_resolve(&ref, d->git_reference);
    return LGReferencePtr(new LGReference(ref));
}

LGOidPtr LGReference::nameToId(LGRepositoryPtr repository, QString name) {
    git_oid oid;
    git_reference_name_to_id(&oid, repository->git_repository(), name.toUtf8().data());
    return LGOidPtr(new LGOid(oid));
}
