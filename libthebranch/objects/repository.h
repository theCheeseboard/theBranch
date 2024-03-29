#ifndef REPOSITORY_H
#define REPOSITORY_H

#include "branch.h"
#include "errorresponse.h"
#include "forward_declares.h"
#include "libthebranch_global.h"
#include <QCoroTask>
#include <QObject>
#include <ranges/trange.h>
#include <tcommandpalette/tcommandpalettescope.h>
#include <tpromise.h>

class BranchModel;
class CommitModel;
class RemotesModel;
class StashesModel;
class CommitSnapIn;
class PushSnapIn;
class PullSnapIn;
class RebaseSnapIn;
class ConflictResolutionSnapIn;

struct RepositoryPrivate;
class RepositoryOperation;
class LIBTHEBRANCH_EXPORT Repository : public QObject,
                                       public ICommitResolvable,
                                       public tbSharedFromThis<Repository> {
        Q_OBJECT
    public:
        ~Repository();

        static RepositoryPtr cloneRepository(QString cloneUrl, QString directory, InformationRequiredCallback callback, QVariantMap options);
        static QCoro::Task<RepositoryPtr> repositoryForDirectoryUi(QWidget* parent);
        static RepositoryPtr repositoryForDirectory(QString directory);
        static RepositoryPtr repositoryForInit(QString directory);
        static QString gitRepositoryRootForDirectory(QString directory);

        enum RepositoryState {
            Invalid,
            Cloning,
            Idle
        };
        RepositoryState state();

        enum class GitState {
            Unknown,
            Idle,
            Merge,
            Revert,
            RevertSequence,
            CherryPick,
            CherryPickSequence,
            Bisect,
            Rebase,
            RebaseInteractive,
            RebaseMerge,
            ApplyMailbox,
            ApplyMailboxOrRebase
        };
        GitState gitState();

        enum class ResetType {
            HardReset,
            MixedReset,
            SoftReset
        };

        QString stateDescription();
        QString stateInformationalText();
        int stateProgress();
        int stateTotalProgress();
        bool stateProvidesProgress();

        ReferencePtr head();
        QList<BranchPtr> branches(THEBRANCH::ListBranchFlags flags);
        BranchPtr createBranch(QString name, CommitPtr target);

        ReferencePtr reference(QString name);
        CommitPtr searchCommit(QString name);

        ErrorResponse setHeadAndCheckout(ReferencePtr reference);
        ErrorResponse detachHead(ICommitResolvablePtr target);
        void resetFileToHead(QString file);

        void reloadRepositoryState();
        IndexPtr index();

        struct StatusItem {
                enum StatusFlag {
                    NoStatusFlag = 0x00,
                    Current = 0x01,
                    New = 0x02,
                    Modified = 0x04,
                    Deleted = 0x08,
                    TypeChanged = 0x10,
                    Renamed = 0x20,
                    Ignored = 0x40,
                    Conflicting = 0x80,

                    FirstStatusFlag = Current,
                    LastStatusFlag = Conflicting
                };

                QString path;
                int flags = NoStatusFlag;
        };

        QList<StatusItem> fileStatus();

        RemotePtr addRemote(QString name, QString url);
        QList<RemotePtr> remotes();

        tRange<TagPtr> tags();
        TagPtr createLightweightTag(QString name, ICommitResolvablePtr commit);

        QCoro::Task<> fetch(QString remote, QStringList refs, InformationRequiredCallback callback);

        QCoro::Task<> stash(QString message);
        QList<StashPtr> stashes();

        ErrorResponse reset(CommitPtr commit, ResetType type);

        QString repositoryPath();

        tCommandPaletteScope* commandPaletteBranches();

    signals:
        void stateChanged();
        void stateDescriptionChanged();
        void stateInformationalTextChanged();
        void stateProgressChanged();
        void repositoryUpdated();

    protected:
        friend CommitModel;
        friend BranchModel;
        friend RemotesModel;
        friend StashesModel;
        friend Merge;
        friend Rebase;
        friend CommitSnapIn;
        friend PushSnapIn;
        friend PullSnapIn;
        friend RebaseSnapIn;
        friend ConflictResolutionSnapIn;
        friend CherryPick;
        friend Index;
        friend Diff;
        friend GitOperation;
        friend RetroactiveRebase;
        friend Revert;
        LGRepositoryPtr git_repository();

    private:
        explicit Repository(QObject* parent = nullptr);
        RepositoryPrivate* d;

        void putRepositoryOperation(RepositoryOperation* operation);
        void updateWatchedDirectories();

        // ICommitResolvable interface
    public:
        CommitPtr resolveToCommit();
};

#endif // REPOSITORY_H
