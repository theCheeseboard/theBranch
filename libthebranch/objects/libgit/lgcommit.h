#ifndef LGCOMMIT_H
#define LGCOMMIT_H

#include "../forward_declares.h"
#include <QObject>

struct git_commit;
struct LGCommitPrivate;
class LGCommit : public QObject,
                 public tbSharedFromThis<LGCommit> {
        Q_OBJECT
    public:
        explicit LGCommit(git_commit* git_commit);
        ~LGCommit();

        static LGCommitPtr lookup(LGRepositoryPtr repo, LGOidPtr oid);

        struct ::git_commit* gitCommit();

        QString message();
        LGSignaturePtr committer();
        LGOidPtr oid();
        LGTreePtr tree();
        int parentCount();
        LGCommitPtr parent(int n);
        QDateTime date();

    signals:

    private:
        LGCommitPrivate* d;
};

#endif // LGCOMMIT_H
