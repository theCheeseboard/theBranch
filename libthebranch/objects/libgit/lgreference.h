#ifndef LGREFERENCE_H
#define LGREFERENCE_H

#include "../errorresponse.h"
#include "../forward_declares.h"
#include <QObject>

struct git_reference;
struct LGReferencePrivate;
class LGReference : public QObject {
        Q_OBJECT
    public:
        LGReference(git_reference* git_reference);
        ~LGReference();

        struct git_reference* gitReference();
        LGReferencePtr dup();
        struct git_reference* takeGitReference();

        ErrorResponse setTarget(LGOidPtr oid, QString reflogMessage);

        bool isBranch();
        QString name();
        QString shorthand();

        LGAnnotatedCommitPtr toAnnotatedCommit(LGRepositoryPtr repository);

        LGReferencePtr resolve();
        static LGOidPtr nameToId(LGRepositoryPtr repository, QString name);

    signals:

    private:
        LGReferencePrivate* d;
};

typedef QSharedPointer<LGReference> LGReferencePtr;

#endif // LGREFERENCE_H