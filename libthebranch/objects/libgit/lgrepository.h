#ifndef LGREPOSITORY_H
#define LGREPOSITORY_H

#include "../errorresponse.h"
#include "../forward_declares.h"
#include <QCoroTask>
#include <QException>
#include <QObject>
#include <functional>

class GitRepositoryOutOfDateException : public QException {
    public:
        void raise() const override { throw *this; }
        GitRepositoryOutOfDateException* clone() const override { return new GitRepositoryOutOfDateException(*this); }
};

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
            MergeRepositoryState
        };

        static LGRepository* open(QString path);
        static QString gitExecutable();

        LGReferencePtr head();
        void setHead(QString head);

        QString path();
        QString workDir();
        QList<LGBranchPtr> branches(THEBRANCH::ListBranchFlags flags);
        LGBranchPtr createBranch(QString name, LGCommitPtr target);

        LGReferencePtr reference(QString name);

        LGIndexPtr index();

        LGTreePtr lookupTree(LGOidPtr oid);

        LGCommitPtr lookupCommit(LGOidPtr oid);
        QCoro::Task<> commit(QString message, LGSignaturePtr committer);

        LGSignaturePtr defaultSignature();

        ErrorResponse checkoutTree(LGReferencePtr revision, QVariantMap options);
        ErrorResponse checkoutIndex(LGIndexPtr index, QVariantMap options);

        LGRemotePtr createRemote(QString name, QString url);
        QList<LGRemotePtr> remotes();

        RepositoryState state();
        void cleanupState();

        QCoro::Task<> push(QString upstreamRemote, QString upstreamBranch, bool setUpstream, bool pushTags, InformationRequiredCallback callback);
        QCoro::Task<> fetch(QString remote, InformationRequiredCallback callback);

        QCoro::Task<std::tuple<int, QString>> runGit(QStringList args);

        git_repository* gitRepository();

    signals:

    private:
        LGRepositoryPrivate* d;

        ErrorResponse performCheckout(std::function<int(git_checkout_options*)> specificCheckout, QVariantMap options);
};

typedef QSharedPointer<LGRepository> LGRepositoryPtr;

#endif // LGREPOSITORY_H
