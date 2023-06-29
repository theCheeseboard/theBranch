#ifndef LGTAG_H
#define LGTAG_H

#include "../forward_declares.h"
#include <QObject>

class LGTag : public QObject,
              public tbSharedFromThis<LGTag> {
        Q_OBJECT
    public:
        explicit LGTag();
        ~LGTag();

        virtual QString name() = 0;
        virtual bool deleteTag(LGRepositoryPtr repo) = 0;
        virtual LGCommitPtr target(LGRepositoryPtr repo) = 0;
    signals:
};

#endif // LGTAG_H
