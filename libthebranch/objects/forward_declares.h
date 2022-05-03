#ifndef FORWARD_DECLARES_H
#define FORWARD_DECLARES_H

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

FORWARD_DECLARE(Repository)
FORWARD_DECLARE(Commit)
FORWARD_DECLARE(Branch)
FORWARD_DECLARE(Reference)

#undef FORWARD_DECLARE

namespace THEBRANCH {
    enum ListBranchFlags {
        AllBranches,
        LocalBranches,
        RemoteBranches
    };
} // namespace THEBRANCH

#endif // FORWARD_DECLARES_H
