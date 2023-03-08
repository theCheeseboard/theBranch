#include "lgobject.h"

#include "lgoid.h"
#include <git2.h>

struct LGObjectPrivate {
        git_object* object;
};

LGObject::LGObject(git_object* object) :
    QObject{nullptr} {
    d = new LGObjectPrivate();
    d->object = object;
}

LGObject::~LGObject() {
    git_object_free(d->object);
    delete d;
}

git_object* LGObject::gitObject() {
    return d->object;
}

LGOidPtr LGObject::oid() {
    const auto* oid = git_object_id(d->object);
    return (new LGOid(oid))->sharedFromThis();
}
