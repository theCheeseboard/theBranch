#ifndef REVERT_H
#define REVERT_H

#include "gitoperation.h"
#include <QObject>

struct RevertPrivate;
class Revert : public GitOperation {
        Q_OBJECT
    public:
        explicit Revert(RepositoryPtr repo, CommitPtr commit, QObject* parent = nullptr);
        ~Revert();

        CommitPtr commit();

        void setMainlineCommit(CommitPtr mainlineCommit);

        enum RevertResult {
            RevertComplete,
            RevertConflict,
            RevertFailed
        };

        RevertResult performRevert();
        ErrorResponse revertFailureReason();

    private:
        RevertPrivate* d;

        // GitOperation interface
    public:
        void finaliseOperation();
};

#endif // REVERT_H
