#include "lgreference.h"

#include <git2.h>

#include "lgannotatedcommit.h"
#include "lgoid.h"
#include "lgrepository.h"

struct LGReferencePrivate {
        git_reference* gitReference;
};

LGReference::LGReference(struct git_reference* git_reference) :
    QObject{nullptr} {
    d = new LGReferencePrivate();
    d->gitReference = git_reference;
}

LGReference::~LGReference() {
    if (d->gitReference) {
        git_reference_free(d->gitReference);
        delete d;
    }
}

git_reference* LGReference::gitReference() {
    return d->gitReference;
}

LGReferencePtr LGReference::dup() {
    struct git_reference* ref;
    ::git_reference_dup(&ref, d->gitReference);
    return (new LGReference(ref))->sharedFromThis();
}

git_reference* LGReference::takeGitReference() {
    struct git_reference* ref = d->gitReference;
    d->gitReference = nullptr;
    return ref;
}

bool LGReference::isEqual(LGReferencePtr other) {
    return git_reference_cmp(d->gitReference, other->d->gitReference) == 0;
}

ErrorResponse LGReference::setTarget(LGOidPtr oid, QString reflogMessage) {
    struct git_reference* newRef;
    if (git_reference_set_target(&newRef, d->gitReference, &oid->gitOid(), reflogMessage.toUtf8().data()) != 0) {
        const git_error* error = git_error_last();
        return ErrorResponse(ErrorResponse::UnspecifiedError, error->message);
    }

    git_reference_free(d->gitReference);
    d->gitReference = newRef;
    return ErrorResponse();
}

bool LGReference::isBranch() {
    return git_reference_is_branch(d->gitReference) || git_reference_is_remote(d->gitReference);
}

QString LGReference::name() {
    return QString::fromUtf8(git_reference_name(d->gitReference));
}

QString LGReference::shorthand() {
    return QString::fromUtf8(git_reference_shorthand(d->gitReference));
}

LGAnnotatedCommitPtr LGReference::toAnnotatedCommit(LGRepositoryPtr repository) {
    git_annotated_commit* out;
    if (git_annotated_commit_from_ref(&out, repository->gitRepository(), d->gitReference) != 0) {
        return nullptr;
    }
    return (new LGAnnotatedCommit(out))->sharedFromThis();
}

LGReferencePtr LGReference::resolve() {
    struct git_reference* ref;
    git_reference_resolve(&ref, d->gitReference);
    return (new LGReference(ref))->sharedFromThis();
}

LGOidPtr LGReference::nameToId(LGRepositoryPtr repository, QString name) {
    git_oid oid;
    git_reference_name_to_id(&oid, repository->gitRepository(), name.toUtf8().data());
    return (new LGOid(oid))->sharedFromThis();
}
