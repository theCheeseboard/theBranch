#ifndef GITOPERATION_H
#define GITOPERATION_H

#include "forward_declares.h"
#include <QObject>

struct GitOperationPrivate;
class GitOperation : public QObject {
        Q_OBJECT
    public:
        explicit GitOperation(RepositoryPtr repo, QObject* parent = nullptr);
        ~GitOperation();

        virtual RepositoryPtr repository();
        virtual void abortOperation();
        virtual void finaliseOperation() = 0;

    signals:

    private:
        GitOperationPrivate* d;
};

#endif // GITOPERATION_H
