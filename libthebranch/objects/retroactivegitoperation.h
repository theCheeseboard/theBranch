#ifndef RETROACTIVEGITOPERATION_H
#define RETROACTIVEGITOPERATION_H

#include "gitoperation.h"
#include <QObject>

class RetroactiveGitOperation : public GitOperation {
        Q_OBJECT
    public:
        Q_INVOKABLE explicit RetroactiveGitOperation(RepositoryPtr repo, QObject* parent = nullptr);

        // GitOperation interface
    public:
        void finaliseOperation();
};

class RetroactiveRebase : public GitOperation {
        Q_OBJECT
    public:
        Q_INVOKABLE explicit RetroactiveRebase(RepositoryPtr repo, QObject* parent = nullptr);

        // GitOperation interface
    public:
        void abortOperation();
        void finaliseOperation();
};

#endif // RETROACTIVEGITOPERATION_H
