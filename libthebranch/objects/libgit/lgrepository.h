#ifndef LGREPOSITORY_H
#define LGREPOSITORY_H

#include "../errorresponse.h"
#include "../forward_declares.h"
#include <QObject>

struct git_repository;
struct LGRepositoryPrivate;
class LGRepository : public QObject {
        Q_OBJECT
    public:
        LGRepository(git_repository* git_repository);
        ~LGRepository();

        static LGRepository* open(QString path);

        LGReferencePtr head();
        void setHead(QString head);

        QString path();
        QList<LGBranchPtr> branches(THEBRANCH::ListBranchFlags flags);

        LGIndexPtr index();

        ErrorResponse checkoutTree(LGReferencePtr revision, QVariantMap options);

        git_repository* git_repository();

    signals:

    private:
        LGRepositoryPrivate* d;
};

typedef QSharedPointer<LGRepository> LGRepositoryPtr;

#endif // LGREPOSITORY_H
