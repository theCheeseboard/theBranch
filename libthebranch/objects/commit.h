#ifndef COMMIT_H
#define COMMIT_H

#include "../libthebranch_global.h"
#include "forward_declares.h"
#include "icommitresolvable.h"
#include <QObject>

class CommitSnapIn;
class CommitModel;
struct CommitPrivate;
class LIBTHEBRANCH_EXPORT Commit : public QObject,
                                   public ICommitResolvable,
                                   public tbSharedFromThis<Commit> {
        Q_OBJECT
    public:
        ~Commit();

        bool equal(CommitPtr other);

        QString commitMessage();
        QString commitHash();
        QString shortCommitHash();
        QString authorName();
        QDateTime date();

        TreePtr tree();

        QList<CommitPtr> parents();
        QList<CommitPtr> history();
        CommitPtr lastCommonAncestor(CommitPtr commit);
        int commitsUntil(CommitPtr commit);
        int missingCommits(CommitPtr commit);
        bool isDescendantOf(CommitPtr commit);
        bool isOrpahan(QList<BranchPtr> branches);

    signals:

    protected:
        friend CommitModel;
        friend Repository;
        friend Branch;
        friend Reference;
        friend CherryPick;
        friend Merge;
        friend CommitSnapIn;
        friend Revert;
        friend Tag;
        static CommitPtr commitForLgCommit(LGRepositoryPtr repo, LGCommitPtr commit);
        LGCommitPtr gitCommit();

    private:
        explicit Commit(QObject* parent = nullptr);
        CommitPrivate* d;

        // ICommitResolvable interface
    private:
        CommitPtr resolveToCommit();
};

typedef QSharedPointer<Commit> CommitPtr;

#endif // COMMIT_H
