#ifndef LGBRANCH_H
#define LGBRANCH_H

#include "../forward_declares.h"
#include <QObject>

struct git_reference;
struct LGBranchPrivate;
class LGBranch : public QObject,
                 public tbSharedFromThis<LGBranch> {
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
        bool rename(QString name);

    signals:

    private:
        LGBranchPrivate* d;
};

#endif // LGBRANCH_H
