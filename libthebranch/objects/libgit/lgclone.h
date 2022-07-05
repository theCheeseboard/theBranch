#ifndef LGCLONE_H
#define LGCLONE_H

#include "../forward_declares.h"
#include "tpromise.h"
#include <QObject>
#include <Task>

struct LGClonePrivate;
class LGClone : public QObject {
        Q_OBJECT
    public:
        explicit LGClone();
        ~LGClone();

        QCoro::Task<> clone(QString cloneUrl, QString directory);

    signals:
        void statusChanged(QString description);
        void progressChanged(int bytesTransferred, int objectsTransferred, int totalObjects);

    private:
        LGClonePrivate* d;
};

#endif // LGCLONE_H
