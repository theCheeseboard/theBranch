#ifndef LGOID_H
#define LGOID_H

#include "../forward_declares.h"
#include <QObject>

struct git_oid;
struct LGOidPrivate;
class LGOid : public QObject,
              public tbSharedFromThis<LGOid> {
        Q_OBJECT
    public:
        explicit LGOid(git_oid git_oid);
        ~LGOid();

        struct ::git_oid& gitOid();
        QString toHex();

    signals:

    private:
        LGOidPrivate* d;
};

#endif // LGOID_H
