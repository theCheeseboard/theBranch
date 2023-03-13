#ifndef TREE_H
#define TREE_H

#include "../libthebranch_global.h"
#include "forward_declares.h"
#include <QObject>

class CommitSnapIn;
class Repository;
struct TreePrivate;
class LIBTHEBRANCH_EXPORT Tree : public QObject,
                                 public tbSharedFromThis<Tree> {
        Q_OBJECT
    public:
        ~Tree();

        BlobPtr blobForPath(QString path);

        QString treeHash();

    signals:

    protected:
        friend Commit;
        friend Diff;
        friend CommitSnapIn;
        friend Repository;
        friend RetroactiveRebase;
        friend Index;
        static TreePtr treeForLgTree(LGRepositoryPtr repo, LGTreePtr tree);
        LGTreePtr gitTree();

    private:
        explicit Tree(QObject* parent = nullptr);
        TreePrivate* d;
};

#endif // TREE_H
