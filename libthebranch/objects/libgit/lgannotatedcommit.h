#ifndef LGANNOTATEDCOMMIT_H
#define LGANNOTATEDCOMMIT_H

#include "../forward_declares.h"
#include <QObject>

struct git_annotated_commit;
struct LGAnnotatedCommitPrivate;
class LGAnnotatedCommit : public QObject,
                          public tbSharedFromThis<LGAnnotatedCommit> {
        Q_OBJECT
    public:
        explicit LGAnnotatedCommit(git_annotated_commit* git_annotated_commit);
        ~LGAnnotatedCommit();

        git_annotated_commit* gitAnnotatedCommit();

        LGOidPtr oid();

    signals:

    private:
        LGAnnotatedCommitPrivate* d;
};

#endif // LGANNOTATEDCOMMIT_H
