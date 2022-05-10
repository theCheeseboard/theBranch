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
FORWARD_DECLARE(LGAnnotatedCommit)
FORWARD_DECLARE(LGIndex)

FORWARD_DECLARE(Repository)
FORWARD_DECLARE(Index)
FORWARD_DECLARE(Commit)
FORWARD_DECLARE(Branch)
FORWARD_DECLARE(Reference)
FORWARD_DECLARE(GitOperation)
FORWARD_DECLARE(Merge)

#undef FORWARD_DECLARE

namespace THEBRANCH {
    enum ListBranchFlags {
        AllBranches,
        LocalBranches,
        RemoteBranches
    };
} // namespace THEBRANCH

#include "errorresponse.h"

#endif // FORWARD_DECLARES_H
