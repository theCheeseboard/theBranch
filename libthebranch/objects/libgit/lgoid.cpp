#include "lgoid.h"

#include <git2.h>

struct LGOidPrivate {
        ::git_oid git_oid;
};

LGOid::LGOid(struct git_oid git_oid) :
    QObject{nullptr} {
    d = new LGOidPrivate;
    d->git_oid = git_oid;
}

LGOid::~LGOid() {
    delete d;
}

git_oid& LGOid::gitOid() {
    return d->git_oid;
}

QString LGOid::toHex() {
    return QByteArray(reinterpret_cast<const char*>(d->git_oid.id), 20).toHex();
}
