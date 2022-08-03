#ifndef LGTREE_H
#define LGTREE_H

#include "../forward_declares.h"
#include <QObject>

struct git_tree;
struct LGTreePrivate;
class LGTree : public QObject,
               public tbSharedFromThis<LGTree> {
        Q_OBJECT
    public:
        explicit LGTree(git_tree* tree);
        ~LGTree();

        git_tree* gitTree();

        LGOidPtr oid();

        LGObjectPtr objectForPath(LGRepositoryPtr repo, QString path);

    signals:

    private:
        LGTreePrivate* d;
};

#endif // LGTREE_H
