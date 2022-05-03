#ifndef LGREFERENCE_H
#define LGREFERENCE_H

#include "../forward_declares.h"
#include <QObject>

struct git_reference;
struct LGReferencePrivate;
class LGReference : public QObject {
        Q_OBJECT
    public:
        LGReference(git_reference* git_reference);
        ~LGReference();

        git_reference* git_reference();

        bool isBranch();
        QString name();
        QString shorthand();

        LGReferencePtr resolve();
        static LGOidPtr nameToId(LGRepositoryPtr repository, QString name);

    signals:

    private:
        LGReferencePrivate* d;
};

typedef QSharedPointer<LGReference> LGReferencePtr;

#endif // LGREFERENCE_H
