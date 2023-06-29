#ifndef ICOMMITRESOLVABLE_H
#define ICOMMITRESOLVABLE_H

#include "forward_declares.h"

class ICommitResolvable {
    public:
        virtual CommitPtr resolveToCommit() = 0;
};

#endif // ICOMMITRESOLVABLE_H
