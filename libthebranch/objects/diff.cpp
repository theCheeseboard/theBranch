#include "diff.h"

#include "commit.h"
#include "libgit/lgblob.h"
#include "libgit/lgoid.h"
#include "libgit/lgrepository.h"
#include "libgit/lgtree.h"
#include "repository.h"
#include "tree.h"
#include <git2.h>

struct DiffPrivate {
        RepositoryPtr repo;
        git_diff* diff;

        QString oldSideDescription;
        QString newSideDescription;
};

Diff::Diff(QObject* parent) :
    QObject{parent} {
    d = new DiffPrivate();
}

Diff::~Diff() {
    git_diff_free(d->diff);
    delete d;
}

DiffPtr Diff::diffTrees(RepositoryPtr repo, TreePtr base, TreePtr compare) {
    git_diff_options diffOptions = GIT_DIFF_OPTIONS_INIT;

    git_diff* diff;
    if (git_diff_tree_to_tree(&diff, repo->git_repository()->gitRepository(), base.isNull() ? nullptr : base->gitTree()->gitTree(), compare.isNull() ? nullptr : compare->gitTree()->gitTree(), &diffOptions) != 0) {
        return nullptr;
    }

    auto* d = new Diff();
    d->d->diff = diff;
    d->d->repo = repo;
    d->d->oldSideDescription = base.isNull() ? tr("Original") : base->treeHash();
    d->d->newSideDescription = compare.isNull() ? tr("Modified") : compare->treeHash();
    return d->sharedFromThis();
}

DiffPtr Diff::diffCommits(RepositoryPtr repo, CommitPtr base, CommitPtr compare) {
    auto diff = diffTrees(repo, base.isNull() ? nullptr : base->tree(), compare.isNull() ? nullptr : compare->tree());
    if (base) diff->d->oldSideDescription = base->shortCommitHash();
    if (compare) diff->d->newSideDescription = compare->shortCommitHash();
    return diff;
}

QList<Diff::DiffFile> Diff::diffFiles() {
    QList<DiffFile> files;
    auto payload = std::tuple<QList<DiffFile>*, Diff*>({&files, this});
    git_diff_foreach(
        d->diff, [](const git_diff_delta* delta, float progress, void* payload) -> int {
            auto [files, thisPtr] = *reinterpret_cast<std::tuple<QList<DiffFile>*, Diff*>*>(payload);
            auto oid1 = (new LGOid(delta->old_file.id))->sharedFromThis();
            auto oid2 = (new LGOid(delta->new_file.id))->sharedFromThis();
            auto blob1 = thisPtr->d->repo->git_repository()->lookupBlob(oid1);
            auto blob2 = thisPtr->d->repo->git_repository()->lookupBlob(oid2);

            DiffFile df;
            df.oldFilePath = delta->old_file.path;
            df.newFilePath = delta->new_file.path;
            df.oldBlob = blob1.isNull() ? QByteArray() : blob1->contents();
            df.newBlob = blob2.isNull() ? QByteArray() : blob2->contents();

            switch (delta->status) {
                case GIT_DELTA_UNMODIFIED:
                case GIT_DELTA_COPIED:
                case GIT_DELTA_UNREADABLE:
                    df.statusFlag = Repository::StatusItem::NoStatusFlag;
                    break;
                case GIT_DELTA_ADDED:
                    df.statusFlag = Repository::StatusItem::New;
                    break;
                case GIT_DELTA_DELETED:
                case GIT_DELTA_UNTRACKED:
                    df.statusFlag = Repository::StatusItem::Deleted;
                    break;
                case GIT_DELTA_MODIFIED:
                    df.statusFlag = Repository::StatusItem::Modified;
                    break;
                case GIT_DELTA_IGNORED:
                    df.statusFlag = Repository::StatusItem::Ignored;
                    break;
                case GIT_DELTA_RENAMED:
                    df.statusFlag = Repository::StatusItem::Renamed;
                    break;
                case GIT_DELTA_TYPECHANGE:
                    df.statusFlag = Repository::StatusItem::TypeChanged;
                    break;
                case GIT_DELTA_CONFLICTED:
                    df.statusFlag = Repository::StatusItem::Conflicting;
                    break;
            }

            files->append(df);
            return 0;
        },
        nullptr, nullptr, nullptr, &payload);

    return files;
}

QString Diff::oldSideDescription() {
    return d->oldSideDescription;
}

QString Diff::newSideDescription() {
    return d->newSideDescription;
}
