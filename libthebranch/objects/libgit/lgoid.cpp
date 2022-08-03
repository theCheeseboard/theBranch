#include "lgoid.h"

#include <git2.h>

struct LGOidPrivate {
        ::git_oid git_oid;
};

LGOid::LGOid(git_oid git_oid) :
    QObject{nullptr} {
    d = new LGOidPrivate;
    git_oid_cpy(&d->git_oid, &git_oid);
}

LGOid::LGOid(const git_oid* git_oid) :
    QObject{nullptr} {
    d = new LGOidPrivate;
    git_oid_cpy(&d->git_oid, git_oid);
}

LGOid::~LGOid() {
    delete d;
}

bool LGOid::equal(LGOidPtr other) {
    return this->toHex() == other->toHex();
}

git_oid& LGOid::gitOid() {
    return d->git_oid;
}

QString LGOid::toHex() {
    return QByteArray(reinterpret_cast<const char*>(d->git_oid.id), 20).toHex();
}

LGOidPtr LGOid::fromHex(QString hex) {
    auto bytes = QByteArray::fromHex(hex.toUtf8());
    if (bytes.length() != 20) return nullptr;

    git_oid oid;
    memcpy(oid.id, bytes.data(), 20);
    return (new LGOid(oid))->sharedFromThis();
}
