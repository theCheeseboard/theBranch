#include "lgblob.h"

#include <git2.h>

struct LGBlobPrivate {
        git_blob* blob;
};

LGBlob::LGBlob(git_blob* blob) :
    QObject{nullptr} {
    d = new LGBlobPrivate();
    d->blob = blob;
}

LGBlob::~LGBlob() {
    git_blob_free(d->blob);
    delete d;
}

QByteArray LGBlob::contents() {
    return QByteArray(reinterpret_cast<const char*>(git_blob_rawcontent(d->blob)), git_blob_rawsize(d->blob));
}
