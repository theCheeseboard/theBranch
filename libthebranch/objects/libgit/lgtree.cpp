#include "lgtree.h"

#include <git2.h>

struct LGTreePrivate {
        git_tree* tree;
};

LGTree::LGTree(git_tree* tree) :
    QObject{nullptr} {
    d = new LGTreePrivate();
    d->tree = tree;
}

LGTree::~LGTree() {
    git_tree_free(d->tree);
    delete d;
}

git_tree* LGTree::gitTree() {
    return d->tree;
}
