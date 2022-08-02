#include "blob.h"

#include "libgit/lgblob.h"

struct BlobPrivate {
        LGBlobPtr blob;
};

Blob::~Blob() {
    delete d;
}

QByteArray Blob::contents() {
    return d->blob->contents();
}

BlobPtr Blob::blobForLgBlob(LGBlobPtr blob) {
    if (!blob) return nullptr;
    auto* b = new Blob();
    b->d->blob = blob;
    return b->sharedFromThis();
}

LGBlobPtr Blob::gitBlob() {
    return d->blob;
}

Blob::Blob(QObject* parent) :
    QObject{parent} {
    d = new BlobPrivate();
}
