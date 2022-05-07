#include "lgreference.h"

#include <git2.h>

#include "lgannotatedcommit.h"
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
    if (d->git_reference) {
        git_reference_free(d->git_reference);
        delete d;
    }
}

git_reference* LGReference::git_reference() {
    return d->git_reference;
}

LGReferencePtr LGReference::dup() {
    struct git_reference* ref;
    ::git_reference_dup(&ref, d->git_reference);
    return LGReferencePtr(new LGReference(ref));
}

git_reference* LGReference::take_git_refernce() {
    struct git_reference* ref = d->git_reference;
    d->git_reference = nullptr;
    return ref;
}

ErrorResponse LGReference::setTarget(LGOidPtr oid, QString reflogMessage) {
    struct git_reference* newRef;
    if (git_reference_set_target(&newRef, d->git_reference, &oid->git_oid(), reflogMessage.toUtf8().data()) != 0) {
        const git_error* error = git_error_last();
        return ErrorResponse(ErrorResponse::UnspecifiedError, error->message);
    }

    git_reference_free(d->git_reference);
    d->git_reference = newRef;
    return ErrorResponse();
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

LGAnnotatedCommitPtr LGReference::toAnnotatedCommit(LGRepositoryPtr repository) {
    git_annotated_commit* out;
    if (git_annotated_commit_from_ref(&out, repository->git_repository(), d->git_reference) != 0) {
        return LGAnnotatedCommitPtr();
    }
    return LGAnnotatedCommitPtr(new LGAnnotatedCommit(out));
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
