#ifndef LGREPOSITORY_H
#define LGREPOSITORY_H

#include "../errorresponse.h"
#include "../forward_declares.h"
#include <QCoroTask>
#include <QException>
#include <QObject>
#include <functional>

struct git_checkout_options;
struct git_repository;
struct LGRepositoryPrivate;
class LGRepository : public QObject,
                     public tbSharedFromThis<LGRepository> {
        Q_OBJECT
    public:
        LGRepository(git_repository* git_repository);
        ~LGRepository();

        enum RepositoryState {
            UnknownRepositoryState,
            IdleRepositoryState,
            MergeRepositoryState,
            RevertRepositoryState,
            RevertSequenceRepositoryState,
            CherryPickRepositoryState,
            CherryPickSequenceRepositoryState,
            BisectRepositoryState,
            RebaseRepositoryState,
            RebaseInteractiveRepositoryState,
            RebaseMergeRepositoryState,
            ApplyMailboxRepositoryState,
            ApplyMailboxOrRebaseRepositoryState
        };

        enum class ObjectType {
            Any,
            Tree,
            Commit,
            Blob,
            Tag
        };

        enum class ResetType {
            HardReset,
            MixedReset,
            SoftReset
        };

        static LGRepositoryPtr init(QString path);
        static LGRepositoryPtr open(QString path);
        static QString gitExecutable();

        LGReferencePtr head();
        void setHead(QString head);
        void detachHead(LGCommitPtr commit);

        QString path();
        QString workDir();
        QList<LGBranchPtr> branches(THEBRANCH::ListBranchFlags flags);
        LGBranchPtr createBranch(QString name, LGCommitPtr target);

        LGReferencePtr reference(QString name);
        LGReferencePtr referenceDwim(QString name);

        LGIndexPtr index();

        LGTreePtr lookupTree(LGOidPtr oid);

        LGCommitPtr lookupCommit(LGOidPtr oid);
        LGOidPtr createCommit(LGSignaturePtr author, LGSignaturePtr committer, QString message, LGTreePtr tree, QList<LGCommitPtr> parents);
        LGOidPtr createCommit(QString refToUpdate, LGSignaturePtr author, LGSignaturePtr committer, QString message, LGTreePtr tree, QList<LGCommitPtr> parents);

        LGBlobPtr lookupBlob(LGOidPtr oid);

        LGSignaturePtr defaultSignature();

        ErrorResponse checkoutTree(LGReferencePtr revision, QVariantMap options);
        ErrorResponse checkoutTree(LGTreePtr tree, QVariantMap options);
        ErrorResponse checkoutIndex(LGIndexPtr index, QVariantMap options);

        LGRemotePtr createRemote(QString name, QString url);
        QList<LGRemotePtr> remotes();

        QList<LGTagPtr> tags();
        LGTagPtr createLightweightTag(QString name, LGCommitPtr target);

        RepositoryState state();
        void cleanupState();

        QCoro::Task<> push(QString upstreamRemote, QString upstreamBranch, bool force, bool setUpstream, bool pushTags, InformationRequiredCallback callback);
        QCoro::Task<> push(QString upstreamRemote, QStringList refs, InformationRequiredCallback callback);
        QCoro::Task<> fetch(QString remote, QStringList refs, InformationRequiredCallback callback);

        QCoro::Task<> stash(QString message, LGSignaturePtr signature);
        QList<LGStashPtr> stashes();

        QCoro::Task<std::tuple<int, QString>> runGit(QStringList args);

        LGObjectPtr lookupObject(LGOidPtr oid, ObjectType type);
        LGObjectPtr revparse(QString revision);

        ErrorResponse reset(LGObjectPtr object, ResetType resetType);

        LGConfigPtr config();

        git_repository* gitRepository();

    signals:

    private:
        LGRepositoryPrivate* d;

        ErrorResponse performCheckout(std::function<int(git_checkout_options*)> specificCheckout, QVariantMap options);
};

typedef QSharedPointer<LGRepository> LGRepositoryPtr;

#endif // LGREPOSITORY_H
