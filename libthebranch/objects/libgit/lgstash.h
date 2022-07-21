#ifndef LGSTASH_H
#define LGSTASH_H

#include "../forward_declares.h"
#include <QObject>

struct LGStashPrivate;
class LGStash : public QObject,
                public tbSharedFromThis<LGStash> {
        Q_OBJECT
    public:
        explicit LGStash(int index, QString message, QObject* parent = nullptr);
        ~LGStash();

        int index();
        QString message();

    signals:

    private:
        LGStashPrivate* d;
};

#endif // LGSTASH_H
