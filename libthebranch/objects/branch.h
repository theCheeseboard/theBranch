#ifndef BRANCH_H
#define BRANCH_H

#include "forward_declares.h"
#include <QObject>

class Repository;
struct BranchPrivate;
class Branch : public QObject {
        Q_OBJECT
    public:
        ~Branch();

        bool equal(ReferencePtr ref);
        bool equal(BranchPtr branch);

        ReferencePtr toReference();

        QString name();

        bool isRemoteBranch();
        QString remoteName();
        QString localBranchName();

        ErrorResponse deleteBranch();
        BranchPtr upstream();

    signals:

    protected:
        friend Repository;
        friend Reference;
        static BranchPtr branchForLgBranch(LGRepositoryPtr repo, LGBranchPtr branch);
        LGBranchPtr gitBranch();

    private:
        explicit Branch(QObject* parent = nullptr);
        BranchPrivate* d;
};

typedef QSharedPointer<Branch> BranchPtr;

#endif // BRANCH_H
