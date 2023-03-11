#include "retroactivegitoperation.h"

#include "commit.h"
#include "libgit/lgrepository.h"
#include "reference.h"
#include "repository.h"
#include "tree.h"
#include <git2.h>

RetroactiveGitOperation::RetroactiveGitOperation(RepositoryPtr repo, QObject* parent) :
    GitOperation{repo, parent} {
}

void RetroactiveGitOperation::finaliseOperation() {
    // noop
}

RetroactiveRebase::RetroactiveRebase(RepositoryPtr repo, QObject* parent) :
    GitOperation{repo, parent} {
}

void RetroactiveRebase::abortOperation() {
    if (repository()->gitState() == Repository::GitState::RebaseInteractive) {
        QDir gitDir(repository()->git_repository()->path());
        QFile origHeadFile(gitDir.absoluteFilePath("rebase-merge/head-name"));
        origHeadFile.open(QFile::ReadOnly);
        auto origHead = origHeadFile.readAll().trimmed();
        origHeadFile.close();

        for (auto branch : repository()->branches(THEBRANCH::LocalBranches)) {
            if (branch->toReference()->name() == origHead) {
                // Abort the ongoing operation
                repository()->git_repository()->cleanupState();
                repository()->git_repository()->checkoutTree(branch->lastCommit()->tree()->gitTree(), {
                                                                                                          {"force", true}
                });
                repository()->setHeadAndCheckout(branch->toReference());
            }
        }

    } else {
        git_rebase_options rebaseOptions = GIT_REBASE_OPTIONS_INIT;
        git_rebase* rebase;
        if (git_rebase_open(&rebase, repository()->git_repository()->gitRepository(), &rebaseOptions) == 0) {
            git_rebase_abort(rebase);
        }
    }
}

void RetroactiveRebase::finaliseOperation() {
}
