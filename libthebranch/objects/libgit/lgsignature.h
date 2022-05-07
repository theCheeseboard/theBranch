#ifndef LGSIGNATURE_H
#define LGSIGNATURE_H

#include "../forward_declares.h"
#include <QObject>

struct git_signature;
struct LGSignaturePrivate;
class LGSignature : public QObject {
        Q_OBJECT
    public:
        explicit LGSignature(git_signature* git_signature);
        explicit LGSignature(const git_signature* git_signature);
        ~LGSignature();

        ::git_signature* gitSignature();

        QString name();
        QString email();

    signals:

    private:
        LGSignaturePrivate* d;
};

#endif // LGSIGNATURE_H
