#ifndef LGCOMMIT_H
#define LGCOMMIT_H

#include "../forward_declares.h"
#include <QObject>

struct git_commit;
struct LGCommitPrivate;
class LGCommit : public QObject {
        Q_OBJECT
    public:
        explicit LGCommit(git_commit* git_commit);
        ~LGCommit();

        static LGCommitPtr lookup(LGRepositoryPtr repo, LGOidPtr oid);

        git_commit* git_commit();

        QString message();
        LGSignaturePtr committer();
        LGOidPtr oid();

    signals:

    private:
        LGCommitPrivate* d;
};

#endif // LGCOMMIT_H
