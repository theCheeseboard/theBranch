#ifndef LGOBJECT_H
#define LGOBJECT_H

#include "../forward_declares.h"
#include <QObject>

struct git_object;
struct LGObjectPrivate;
class LGObject : public QObject,
                 public tbSharedFromThis<LGObject> {
        Q_OBJECT
    public:
        explicit LGObject(git_object* object);
        ~LGObject();

        enum class Type {
            Unknown,
            Commit,
            Tag,
            Blob,
            Tree
        };

        struct ::git_object* gitObject();

        LGOidPtr oid();
        Type type();

    signals:

    private:
        LGObjectPrivate* d;
};

#endif // LGOBJECT_H
