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

LGObject::Type LGObject::type() {
    switch (git_object_type(d->object)) {
        case GIT_OBJECT_TAG:
            return Type::Tag;
        case GIT_OBJECT_BLOB:
            return Type::Blob;
        case GIT_OBJECT_COMMIT:
            return Type::Commit;
        case GIT_OBJECT_TREE:
            return Type::Tree;
        case GIT_OBJECT_ANY:
        case GIT_OBJECT_INVALID:
        case GIT_OBJECT_OFS_DELTA:
        case GIT_OBJECT_REF_DELTA:
            return Type::Unknown;
            break;
    }
}
