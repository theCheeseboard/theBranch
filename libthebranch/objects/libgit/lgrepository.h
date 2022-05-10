#ifndef LGREPOSITORY_H
#define LGREPOSITORY_H

#include "../errorresponse.h"
#include "../forward_declares.h"
#include <QObject>
#include <functional>

struct git_checkout_options;
struct git_repository;
struct LGRepositoryPrivate;
class LGRepository : public QObject {
        Q_OBJECT
    public:
        LGRepository(git_repository* git_repository);
        ~LGRepository();

        enum RepositoryState {
            UnknownRepositoryState,
            IdleRepositoryState,
            MergeRepositoryState
        };

        static LGRepository* open(QString path);

        LGReferencePtr head();
        void setHead(QString head);

        QString path();
        QList<LGBranchPtr> branches(THEBRANCH::ListBranchFlags flags);
        LGBranchPtr createBranch(QString name, LGCommitPtr target);

        LGReferencePtr reference(QString name);

        LGIndexPtr index();

        ErrorResponse checkoutTree(LGReferencePtr revision, QVariantMap options);
        ErrorResponse checkoutIndex(LGIndexPtr index, QVariantMap options);

        RepositoryState state();
        void cleanupState();

        git_repository* gitRepository();

    signals:

    private:
        LGRepositoryPrivate* d;

        ErrorResponse performCheckout(std::function<int(git_checkout_options*)> specificCheckout, QVariantMap options);
};

typedef QSharedPointer<LGRepository> LGRepositoryPtr;

#endif // LGREPOSITORY_H
