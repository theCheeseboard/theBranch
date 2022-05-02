#ifndef REPOSITORYCLONEOPERATION_H
#define REPOSITORYCLONEOPERATION_H

#include "repositoryoperation.h"

struct RepositoryCloneOperationPrivate;
class RepositoryCloneOperation : public RepositoryOperation {
        Q_OBJECT
    public:
        explicit RepositoryCloneOperation(QString cloneUrl, QString directory, QVariantMap options, QObject* parent = nullptr);
        ~RepositoryCloneOperation();

    signals:

    private:
        RepositoryCloneOperationPrivate* d;

        // RepositoryOperation interface
    public:
        Repository::RepositoryState state();
        QString stateDescription();
        QString stateInformationalText();
        int progress();
        int totalProgress();
        bool providesProgress();
};

#endif // REPOSITORYCLONEOPERATION_H
