#ifndef REPOSITORY_H
#define REPOSITORY_H

#include "libthebranch_global.h"
#include <QObject>
#include <tpromise.h>

struct RepositoryPrivate;
class RepositoryOperation;
class LIBTHEBRANCH_EXPORT Repository : public QObject {
        Q_OBJECT
    public:
        ~Repository();

        enum RepositoryState {
            Invalid,
            Cloning,
            Idle
        };
        RepositoryState state();

        QString stateDescription();
        QString stateInformationalText();
        int stateProgress();
        int stateTotalProgress();
        bool stateProvidesProgress();

        static Repository* cloneRepository(QString cloneUrl, QString directory, QVariantMap options);
        static tPromise<Repository*>* repositoryForDirectoryUi(QWidget* parent);
        static Repository* repositoryForDirectory(QString directory);
        static QString gitRepositoryRootForDirectory(QString directory);

        void reloadRepositoryState();

        QString repositoryPath();

    signals:
        void stateChanged();
        void stateDescriptionChanged();
        void stateInformationalTextChanged();
        void stateProgressChanged();

    private:
        explicit Repository(QObject* parent = nullptr);
        RepositoryPrivate* d;

        void putRepositoryOperation(RepositoryOperation* operation);
};

#endif // REPOSITORY_H
