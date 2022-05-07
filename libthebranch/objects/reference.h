#ifndef REFERENCE_H
#define REFERENCE_H

#include "forward_declares.h"
#include <QObject>

struct ReferencePrivate;
class Reference : public QObject {
        Q_OBJECT
    public:
        ~Reference();

        BranchPtr asBranch();

        LGReferencePtr git_reference();

        QString name();
        QString shorthand();

    signals:

    protected:
        friend Branch;
        friend Repository;
        static ReferencePtr referenceForLgReference(LGReferencePtr reference);

    private:
        explicit Reference(QObject* parent = nullptr);
        ReferencePrivate* d;
};

#endif // REFERENCE_H