#ifndef LGBLOB_H
#define LGBLOB_H

#include "../forward_declares.h"
#include <QObject>

struct git_blob;
struct LGBlobPrivate;
class LGBlob : public QObject,
               public tbSharedFromThis<LGBlob> {
        Q_OBJECT
    public:
        explicit LGBlob(git_blob* blob);
        ~LGBlob();

        QByteArray contents();

    signals:

    private:
        LGBlobPrivate* d;
};

#endif // LGBLOB_H
