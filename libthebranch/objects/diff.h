#ifndef DIFF_H
#define DIFF_H

#include "forward_declares.h"
#include "repository.h"

struct DiffPrivate;
class Diff : public QObject,
             public tbSharedFromThis<Diff> {
        Q_OBJECT
    public:
        ~Diff();

        struct DiffFile {
                QString oldFilePath;
                QString newFilePath;
                QByteArray oldBlob;
                QByteArray newBlob;

                Repository::StatusItem::StatusFlag statusFlag;
        };

        static DiffPtr diffTrees(RepositoryPtr repo, TreePtr base, TreePtr compare);
        static DiffPtr diffCommits(RepositoryPtr repo, CommitPtr base, CommitPtr compare);

        QList<DiffFile> diffFiles();
        QString oldSideDescription();
        QString newSideDescription();

    signals:

    private:
        explicit Diff(QObject* parent = nullptr);
        DiffPrivate* d;
};

#endif // DIFF_H
