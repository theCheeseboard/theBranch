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
            MergeNotPossibleUnknownReason
        };

        MergeNotPossibleReason mergeNotPossibleReason();

        QStringList conflictingFiles();
        void abortOperation();

    signals:

    private:
        MergePrivate* d;
};

#endif // MERGE_H
