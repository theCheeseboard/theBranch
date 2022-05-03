#ifndef REPOSITORYOPERATION_H
#define REPOSITORYOPERATION_H

#include "../repository.h"
#include <QObject>

class RepositoryOperation : public QObject {
        Q_OBJECT
    public:
        explicit RepositoryOperation(QObject* parent = nullptr);

        virtual Repository::RepositoryState state() = 0;
        virtual QString stateDescription() = 0;
        virtual QString stateInformationalText() = 0;
        virtual int progress() = 0;
        virtual int totalProgress() = 0;
        virtual bool providesProgress() = 0;

    signals:
        void stateChanged();
        void stateDescriptionChanged();
        void stateInformationalTextChanged();
        void progressChanged();
        void done();

        void putRepository(LGRepositoryPtr repository);
        void reloadRepository();
};

#endif // REPOSITORYOPERATION_H
