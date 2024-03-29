#ifndef BRANCH_H
#define BRANCH_H

#include "../libthebranch_global.h"
#include "forward_declares.h"
#include "icommitresolvable.h"
#include <QCoroTask>
#include <QObject>

class Repository;
struct BranchPrivate;
class LIBTHEBRANCH_EXPORT Branch : public QObject,
                                   public ICommitResolvable,
                                   public tbSharedFromThis<Branch> {
        Q_OBJECT
    public:
        ~Branch();

        bool equal(ReferencePtr ref);
        bool equal(BranchPtr branch);

        ReferencePtr toReference();
        CommitPtr lastCommit();

        QString name();

        bool isRemoteBranch();
        QString remoteName();
        QString localBranchName();

        ErrorResponse deleteBranch();
        QCoro::Task<> deleteRemoteBranch();
        BranchPtr upstream();

        ErrorResponse rename(QString name);

    signals:

    protected:
        friend Repository;
        friend Reference;
        static BranchPtr branchForLgBranch(LGRepositoryPtr repo, LGBranchPtr branch);
        LGBranchPtr gitBranch();

    private:
        explicit Branch(QObject* parent = nullptr);
        BranchPrivate* d;

        // ICommitResolvable interface
    private:
        CommitPtr resolveToCommit();
};

#endif // BRANCH_H
