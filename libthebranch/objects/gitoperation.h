#ifndef GITOPERATION_H
#define GITOPERATION_H

#include "forward_declares.h"
#include <QObject>

class GitOperation : public QObject {
        Q_OBJECT
    public:
        explicit GitOperation(QObject* parent = nullptr);

        virtual RepositoryPtr repository() = 0;
        virtual void abortOperation() = 0;
        virtual void finaliseOperation() = 0;

    signals:
};

#endif // GITOPERATION_H
