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

        QString name();

    signals:

    protected:
        friend Repository;
        friend Reference;
        static BranchPtr branchForLgBranch(LGBranchPtr branch);

    private:
        explicit Branch(QObject* parent = nullptr);
        BranchPrivate* d;
};

typedef QSharedPointer<Branch> BranchPtr;

#endif // BRANCH_H
