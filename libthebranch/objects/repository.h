#ifndef REPOSITORY_H
#define REPOSITORY_H

#include "branch.h"
#include "forward_declares.h"
#include "libthebranch_global.h"
#include <QObject>
#include <tpromise.h>

class BranchModel;
class CommitModel;
struct RepositoryPrivate;
class RepositoryOperation;
class LIBTHEBRANCH_EXPORT Repository : public QObject {
        Q_OBJECT
    public:
        ~Repository();

        static RepositoryPtr cloneRepository(QString cloneUrl, QString directory, QVariantMap options);
        static tPromise<RepositoryPtr>* repositoryForDirectoryUi(QWidget* parent);
        static RepositoryPtr repositoryForDirectory(QString directory);
        static QString gitRepositoryRootForDirectory(QString directory);

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

        ReferencePtr head();
        QList<BranchPtr> branches(THEBRANCH::ListBranchFlags flags);

        void reloadRepositoryState();

        QString repositoryPath();

    signals:
        void stateChanged();
        void stateDescriptionChanged();
        void stateInformationalTextChanged();
        void stateProgressChanged();
        void repositoryUpdated();

    protected:
        friend CommitModel;
        friend BranchModel;
        LGRepositoryPtr git_repository();

    private:
        explicit Repository(QObject* parent = nullptr);
        RepositoryPrivate* d;

        void putRepositoryOperation(RepositoryOperation* operation);
        void updateWatchedDirectories();
};

#endif // REPOSITORY_H
