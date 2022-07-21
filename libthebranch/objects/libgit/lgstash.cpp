#include "lgstash.h"

struct LGStashPrivate {
        int index;
        QString message;
};

LGStash::LGStash(int index, QString message, QObject* parent) :
    QObject{parent} {
    d = new LGStashPrivate();
    d->index = index;
    d->message = message;
}

LGStash::~LGStash() {
    delete d;
}

int LGStash::index() {
    return d->index;
}

QString LGStash::message() {
    return d->message;
}
