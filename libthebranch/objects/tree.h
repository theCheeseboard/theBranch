#ifndef TREE_H
#define TREE_H

#include "../libthebranch_global.h"
#include "forward_declares.h"
#include <QObject>

struct TreePrivate;
class LIBTHEBRANCH_EXPORT Tree : public QObject,
                                 public tbSharedFromThis<Tree> {
        Q_OBJECT
    public:
        ~Tree();

        BlobPtr blobForPath(QString path);

    signals:

    protected:
        friend Commit;
        static TreePtr treeForLgTree(LGRepositoryPtr repo, LGTreePtr tree);

    private:
        explicit Tree(QObject* parent = nullptr);
        TreePrivate* d;
};

#endif // TREE_H
