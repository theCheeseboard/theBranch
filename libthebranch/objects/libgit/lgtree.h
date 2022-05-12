#ifndef LGTREE_H
#define LGTREE_H

#include "../forward_declares.h"
#include <QObject>

struct git_tree;
struct LGTreePrivate;
class LGTree : public QObject {
        Q_OBJECT
    public:
        explicit LGTree(git_tree* tree);
        ~LGTree();

        git_tree* gitTree();

    signals:

    private:
        LGTreePrivate* d;
};

#endif // LGTREE_H
