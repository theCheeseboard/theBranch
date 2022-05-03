#include "lgrepository.h"

#include "lgbranch.h"
#include "lgreference.h"
#include <git2.h>

struct LGRepositoryPrivate {
        git_repository* git_repository;
};

LGRepository::LGRepository(struct git_repository* git_repository) :
    QObject{nullptr} {
    d = new LGRepositoryPrivate();
    d->git_repository = git_repository;
}

LGRepository* LGRepository::open(QString path) {
    struct git_repository* repo;
    if (git_repository_open(&repo, path.toUtf8().data()) != 0) return nullptr;
    return new LGRepository(repo);
}

QString LGRepository::path() {
    return QString::fromUtf8(git_repository_path(d->git_repository));
}

LGReferencePtr LGRepository::head() {
    git_reference* head;
    if (git_repository_head(&head, d->git_repository) != 0) return nullptr;
    return LGReferencePtr(new LGReference(head));
}

QList<LGBranchPtr> LGRepository::branches(THEBRANCH::ListBranchFlags flags) {
    QList<LGBranchPtr> branches;
    git_branch_t gitFlags;
    if (flags == THEBRANCH::AllBranches) gitFlags = GIT_BRANCH_ALL;
    if (flags == THEBRANCH::RemoteBranches) gitFlags = GIT_BRANCH_REMOTE;
    if (flags == THEBRANCH::LocalBranches) gitFlags = GIT_BRANCH_LOCAL;

    git_branch_iterator* iterator;
    if (git_branch_iterator_new(&iterator, d->git_repository, gitFlags) != 0) return {};

    git_reference* reference;
    git_branch_t type;
    while (!git_branch_next(&reference, &type, iterator)) {
        branches.append(LGBranchPtr(new LGBranch(reference)));
    }
    git_branch_iterator_free(iterator);
    return branches;
}

LGRepository::~LGRepository() {
    git_repository_free(d->git_repository);
    delete d;
}

git_repository* LGRepository::git_repository() {
    return d->git_repository;
}
