#ifndef CHERRYPICK_H
#define CHERRYPICK_H

#include "gitoperation.h"

struct CherryPickPrivate;
class CherryPick : public GitOperation {
        Q_OBJECT
    public:
        explicit CherryPick(RepositoryPtr repo, CommitPtr commit, QObject* parent = nullptr);
        ~CherryPick();

        CommitPtr commit();

        void setMainlineCommit(CommitPtr mainlineCommit);

        enum CherryPickResult {
            CherryPickComplete,
            CherryPickConflict,
            CherryPickFailed
        };

        CherryPickResult performCherryPick();
        ErrorResponse cherryPickFailureReason();

    signals:

    private:
        CherryPickPrivate* d;

        // GitOperation interface
    public:
        RepositoryPtr repository();
        void finaliseOperation();
};

#endif // CHERRYPICK_H
