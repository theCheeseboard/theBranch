#ifndef STASH_H
#define STASH_H

#include "forward_declares.h"
#include <QObject>

struct StashPrivate;
class Stash : public QObject,
              public tbSharedFromThis<Stash> {
        Q_OBJECT
    public:
        ~Stash();

        QString message();

        bool apply();
        void drop();

    signals:

    protected:
        friend Repository;
        static StashPtr stashForLgStash(LGStashPtr stash, LGRepositoryPtr repo);

    private:
        explicit Stash(QObject* parent = nullptr);

        StashPrivate* d;
};

#endif // STASH_H
