#ifndef LGLIGHTWEIGHTTAG_H
#define LGLIGHTWEIGHTTAG_H

#include "../forward_declares.h"
#include "lgtag.h"
#include <QObject>

struct LGLightweightTagPrivate;
class LGLightweightTag : public LGTag,
                         public tbSharedFromThis<LGLightweightTag> {
        Q_OBJECT
    public:
        explicit LGLightweightTag(QString fullName, LGOidPtr target, QObject* parent = nullptr);
        ~LGLightweightTag();

    signals:

    private:
        LGLightweightTagPrivate* d;

        // LGTag interface
    public:
        QString name();
        bool deleteTag(LGRepositoryPtr repo);
        LGCommitPtr target(LGRepositoryPtr repo);
};

#endif // LGLIGHTWEIGHTTAG_H
