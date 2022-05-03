#ifndef LGREPOSITORY_H
#define LGREPOSITORY_H

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

        QString path();
        LGReferencePtr head();
        QList<LGBranchPtr> branches(THEBRANCH::ListBranchFlags flags);

        git_repository* git_repository();

    signals:

    private:
        LGRepositoryPrivate* d;
};

typedef QSharedPointer<LGRepository> LGRepositoryPtr;

#endif // LGREPOSITORY_H
