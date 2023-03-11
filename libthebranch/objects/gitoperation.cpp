#include "gitoperation.h"

#include "libgit/lgrepository.h"
#include "repository.h"

struct GitOperationPrivate {
        RepositoryPtr repo;
};

GitOperation::GitOperation(RepositoryPtr repo, QObject* parent) :
    QObject{parent} {
    d = new GitOperationPrivate();
    d->repo = repo;
}

GitOperation::~GitOperation() {
    delete d;
}

RepositoryPtr GitOperation::repository() {
    return d->repo;
}

void GitOperation::abortOperation() {
    // Abort the ongoing operation
    d->repo->git_repository()->cleanupState();
    d->repo->git_repository()->checkoutTree(d->repo->git_repository()->head(), {
                                                                                   {"force", true}
    });
}
