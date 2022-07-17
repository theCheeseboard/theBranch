#ifndef LGREFERENCE_H
#define LGREFERENCE_H

#include "../errorresponse.h"
#include "../forward_declares.h"
#include <QObject>

struct git_reference;
struct LGReferencePrivate;
class LGReference : public QObject,
                    public tbSharedFromThis<LGReference> {
        Q_OBJECT
    public:
        LGReference(git_reference* git_reference);
        ~LGReference();

        struct git_reference* gitReference();
        LGReferencePtr dup();
        struct git_reference* takeGitReference();

        bool isEqual(LGReferencePtr other);

        ErrorResponse setTarget(LGOidPtr oid, QString reflogMessage);

        bool isBranch();
        QString name();
        QString shorthand();
        QString symbolicTarget();

        LGAnnotatedCommitPtr toAnnotatedCommit(LGRepositoryPtr repository);

        LGReferencePtr resolve();
        LGOidPtr target();
        static LGOidPtr nameToId(LGRepositoryPtr repository, QString name);

    signals:

    private:
        LGReferencePrivate* d;
};

typedef QSharedPointer<LGReference> LGReferencePtr;

#endif // LGREFERENCE_H
