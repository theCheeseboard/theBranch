#ifndef FORWARD_DECLARES_H
#define FORWARD_DECLARES_H

#include <QCoroTask>
#include <QSharedPointer>

#define FORWARD_DECLARE(cls) \
    class cls;               \
    typedef QSharedPointer<cls> cls##Ptr;

FORWARD_DECLARE(LGRepository)
FORWARD_DECLARE(LGReference)
FORWARD_DECLARE(LGRevwalk)
FORWARD_DECLARE(LGOid)
FORWARD_DECLARE(LGCommit)
FORWARD_DECLARE(LGSignature)
FORWARD_DECLARE(LGClone)
FORWARD_DECLARE(LGBranch)
FORWARD_DECLARE(LGAnnotatedCommit)
FORWARD_DECLARE(LGIndex)
FORWARD_DECLARE(LGTree)
FORWARD_DECLARE(LGRemote)
FORWARD_DECLARE(LGActiveRemote)
FORWARD_DECLARE(LGStash)
FORWARD_DECLARE(LGObject)
FORWARD_DECLARE(LGBlob)

FORWARD_DECLARE(Repository)
FORWARD_DECLARE(Index)
FORWARD_DECLARE(Commit)
FORWARD_DECLARE(Branch)
FORWARD_DECLARE(Reference)
FORWARD_DECLARE(GitOperation)
FORWARD_DECLARE(Merge)
FORWARD_DECLARE(PullMerge)
FORWARD_DECLARE(Rebase)
FORWARD_DECLARE(PullRebase)
FORWARD_DECLARE(StatusList)
FORWARD_DECLARE(Remote)
FORWARD_DECLARE(Tree)
FORWARD_DECLARE(Stash)
FORWARD_DECLARE(Blob)

#undef FORWARD_DECLARE

typedef std::function<QCoro::Task<QVariantMap>(QVariantMap)> InformationRequiredCallback;

namespace THEBRANCH {
    enum ListBranchFlags {
        AllBranches,
        LocalBranches,
        RemoteBranches
    };
} // namespace THEBRANCH

template<typename T> class tbSharedFromThis : public QEnableSharedFromThis<T> {
    public:
        QSharedPointer<T> sharedFromThis() {
            auto ptr = QEnableSharedFromThis<T>::sharedFromThis();
            if (ptr) return ptr;
            return QSharedPointer<T>(static_cast<T*>(this));
        }
};

#include "errorresponse.h"

#endif // FORWARD_DECLARES_H
