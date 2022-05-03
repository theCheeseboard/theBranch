#include "lgsignature.h"

#include <git2.h>

struct LGSignaturePrivate {
        union {
                git_signature* signature;
                const git_signature* constSignature;
        };

        bool isConst = false;
};

LGSignature::LGSignature(struct git_signature* git_signature) {
    d = new LGSignaturePrivate();
    d->signature = git_signature;
    d->isConst = false;
}

LGSignature::LGSignature(const struct git_signature* git_signature) :
    QObject{nullptr} {
    d = new LGSignaturePrivate();
    d->constSignature = git_signature;
    d->isConst = true;
}

LGSignature::~LGSignature() {
    if (!d->isConst) git_signature_free(d->signature);
    delete d;
}

git_signature* LGSignature::git_signature() {
    if (d->isConst) return nullptr;
    return d->signature;
}

QString LGSignature::name() {
    const struct git_signature* sig = d->isConst ? d->constSignature : d->signature;
    return QString::fromUtf8(sig->name);
}

QString LGSignature::email() {
    const struct git_signature* sig = d->isConst ? d->constSignature : d->signature;
    return QString::fromUtf8(sig->email);
}
