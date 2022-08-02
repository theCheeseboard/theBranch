#ifndef BLOB_H
#define BLOB_H

#include "forward_declares.h"
#include <QObject>

struct BlobPrivate;
class Blob : public QObject,
             public tbSharedFromThis<Blob> {
        Q_OBJECT
    public:
        ~Blob();

        QByteArray contents();

    signals:

    protected:
        friend Tree;
        static BlobPtr blobForLgBlob(LGBlobPtr blob);
        LGBlobPtr gitBlob();

    private:
        explicit Blob(QObject* parent = nullptr);
        BlobPrivate* d;
};

#endif // BLOB_H
