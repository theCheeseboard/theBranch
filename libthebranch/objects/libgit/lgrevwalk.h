#ifndef LGREVWALK_H
#define LGREVWALK_H

#include "../forward_declares.h"
#include <QObject>

struct git_revwalk;
struct LGRevwalkPrivate;
class LGRevwalk : public QObject {
        Q_OBJECT
    public:
        explicit LGRevwalk(git_revwalk* git_revwalk);
        ~LGRevwalk();

        static LGRevwalkPtr revwalk_new(LGRepositoryPtr repo);
        void sorting(uint sortMode);
        void push(LGOidPtr oid);

        QList<LGOidPtr> walk();

        git_revwalk* git_revwalk();

    signals:

    private:
        LGRevwalkPrivate* d;
};

#endif // LGREVWALK_H
