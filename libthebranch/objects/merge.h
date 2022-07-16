#ifndef MERGE_H
#define MERGE_H

#include "errorresponse.h"
#include "forward_declares.h"
#include "gitoperation.h"

struct MergePrivate;
class Merge : public GitOperation {
        Q_OBJECT
    public:
        explicit Merge(RepositoryPtr repo, BranchPtr branch, QObject* parent = nullptr);
        ~Merge();

        enum MergeType {
            UpToDate,
            FastForward,
            MergeCommit,
            MergeNotPossible
        };

        MergeType mergeType();

        enum MergeResult {
            MergeComplete,
            MergeConflict,
            MergeFailed
        };

        MergeResult performMerge();
        ErrorResponse mergeFailureReason();

        enum MergeNotPossibleReason {
            MergeNotPossibleBecauseHeadDetached,
            MergeNotPossibleBecauseRepositoryNotIdle,
            MergeNotPossibleUnknownReason
        };

        MergeNotPossibleReason mergeNotPossibleReason();

        RepositoryPtr repository();
        void abortOperation();
        void finaliseOperation();

    signals:

    private:
        MergePrivate* d;
};

class PullMerge : public Merge {
        Q_OBJECT
    public:
        explicit PullMerge(RepositoryPtr repo, BranchPtr branch, QObject* parent = nullptr);
};

#endif // MERGE_H
