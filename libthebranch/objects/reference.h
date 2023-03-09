#ifndef REFERENCE_H
#define REFERENCE_H

#include "../libthebranch_global.h"
#include "forward_declares.h"
#include <QObject>

struct ReferencePrivate;
class LIBTHEBRANCH_EXPORT Reference : public QObject,
                                      public tbSharedFromThis<Reference> {
        Q_OBJECT
    public:
        ~Reference();

        BranchPtr asBranch();
        CommitPtr asCommit();

        LGReferencePtr git_reference();

        QString name();
        QString shorthand();
        QString symbolicTarget();

    signals:

    protected:
        friend Branch;
        friend Repository;
        static ReferencePtr referenceForLgReference(LGRepositoryPtr repo, LGReferencePtr reference);

    private:
        explicit Reference(QObject* parent = nullptr);
        ReferencePrivate* d;
};

#endif // REFERENCE_H
