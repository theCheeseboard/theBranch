#ifndef LGCLONE_H
#define LGCLONE_H

#include "../forward_declares.h"
#include "tpromise.h"
#include <QCoroTask>
#include <QObject>

struct LGClonePrivate;
class LGClone : public QObject,
                public tbSharedFromThis<LGClone> {
        Q_OBJECT
    public:
        explicit LGClone();
        ~LGClone();

        QCoro::Task<> clone(QString cloneUrl, QString directory);

        void setInformationRequiredCallback(InformationRequiredCallback callback);
        void setBranch(QString branch);

    signals:
        void statusChanged(QString description);
        void progressChanged(int bytesTransferred, int objectsTransferred, int totalObjects);

    private:
        LGClonePrivate* d;
};

#endif // LGCLONE_H
