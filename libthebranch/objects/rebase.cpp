/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2022 Victor Tran
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * *************************************/
#include "rebase.h"

#include "libgit/lgannotatedcommit.h"
#include "libgit/lgindex.h"
#include "libgit/lgreference.h"
#include "libgit/lgrepository.h"
#include "libgit/lgsignature.h"
#include "reference.h"
#include "repository.h"
#include <git2.h>

struct RebasePrivate {
        RepositoryPtr repo;
        BranchPtr from;
        BranchPtr onto;
        LGAnnotatedCommitPtr fromAnnotatedCommit;
        LGAnnotatedCommitPtr ontoAnnotatedCommit;
        LGSignaturePtr signature;

        git_rebase* rebase;
        git_rebase_operation* currentOperation = nullptr;
        bool rebaseComplete = false;
};

Rebase::Rebase(RepositoryPtr repo, BranchPtr from, BranchPtr onto, QObject* parent) :
    GitOperation{repo, parent} {
    d = new RebasePrivate();
    d->repo = repo;
    d->from = from;
    d->onto = onto;

    d->fromAnnotatedCommit = from->toReference()->git_reference()->toAnnotatedCommit(d->repo->git_repository());
    d->ontoAnnotatedCommit = onto->toReference()->git_reference()->toAnnotatedCommit(d->repo->git_repository());

    if (git_rebase_init(&d->rebase, d->repo->git_repository()->gitRepository(), d->fromAnnotatedCommit->gitAnnotatedCommit(), nullptr, d->ontoAnnotatedCommit->gitAnnotatedCommit(), nullptr) != 0) {
        d->rebase = nullptr;
    }
}

Rebase::~Rebase() {
    if (d->rebase) {
        if (!d->rebaseComplete) git_rebase_abort(d->rebase);
        git_rebase_free(d->rebase);
    }
    delete d;
}

BranchPtr Rebase::from() {
    return d->from;
}

BranchPtr Rebase::onto() {
    return d->onto;
}

void Rebase::setSignature(LGSignaturePtr signature) {
    d->signature = signature;
}

bool Rebase::isValid() {
    return d->rebase;
}

int Rebase::operationCount() {
    if (!d->rebase) return 0;
    return git_rebase_operation_entrycount(d->rebase);
}

int Rebase::currentOperation() {
    if (!d->rebase) return 0;
    return git_rebase_operation_current(d->rebase);
}

RepositoryPtr Rebase::repository() {
    return d->repo;
}

void Rebase::abortOperation() {
    if (!d->rebaseComplete) git_rebase_abort(d->rebase);
    d->rebaseComplete = true;
    emit rebaseComplete();
}

void Rebase::finaliseOperation() {
    if (d->currentOperation) {
        git_oid oid;
        git_rebase_commit(&oid, d->rebase, nullptr, d->signature->gitSignature(), nullptr, nullptr);
        d->currentOperation = nullptr;
    }

    git_rebase_next(&d->currentOperation, d->rebase);
    while (d->currentOperation) {
        LGIndexPtr index = d->repo->git_repository()->index();
        if (index->hasConflicts()) {
            // Rebasing resulted in conflicts
            emit conflictEncountered();
            return;
        }

        git_oid oid;
        git_rebase_commit(&oid, d->rebase, nullptr, d->signature->gitSignature(), nullptr, nullptr);
        d->currentOperation = nullptr;

        // Continue rebasing until we are done
        git_rebase_next(&d->currentOperation, d->rebase);
    }

    git_rebase_finish(d->rebase, d->signature->gitSignature());
    d->rebaseComplete = true;
    emit rebaseComplete();
}

PullRebase::PullRebase(RepositoryPtr repo, BranchPtr from, BranchPtr onto, QObject* parent) :
    Rebase(repo, from, onto, parent) {
}
