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

        struct git_reference* git_reference();
        struct git_reference* take_git_reference();

        LGBranchPtr dup();

        QString name();

    signals:

    private:
        LGReferencePrivate* d;
};

#endif // LGBRANCH_H
