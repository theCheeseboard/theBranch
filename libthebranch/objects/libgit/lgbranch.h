#ifndef LGBRANCH_H
#define LGBRANCH_H

#include "../forward_declares.h"
#include <QObject>

struct git_reference;
struct LGReferencePrivate;
class LGBranch : public QObject {
        Q_OBJECT
    public:
        explicit LGBranch(git_reference* git_reference);
        ~LGBranch();

        struct ::git_reference* gitReference();
        struct ::git_reference* takeGitReference();

        LGBranchPtr dup();

        QString name();

        bool isRemoteBranch();
        QString remoteName(LGRepositoryPtr repo);
        QString localBranchName(LGRepositoryPtr repo);

        LGBranchPtr upstream();
        bool setUpstream(LGBranchPtr upstream);

        bool deleteBranch();

    signals:

    private:
        LGReferencePrivate* d;
};

#endif // LGBRANCH_H
