#include "lgtree.h"

#include "lgobject.h"
#include "lgrepository.h"
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

LGObjectPtr LGTree::objectForPath(LGRepositoryPtr repo, QString path) {
    git_tree_entry* treeEntry;
    if (git_tree_entry_bypath(&treeEntry, d->tree, path.toUtf8().data()) != 0) {
        return nullptr;
    }

    git_object* object;
    if (git_tree_entry_to_object(&object, repo->gitRepository(), treeEntry) != 0) {
        git_tree_entry_free(treeEntry);
        return nullptr;
    }

    git_tree_entry_free(treeEntry);

    return (new LGObject(object))->sharedFromThis();
}
