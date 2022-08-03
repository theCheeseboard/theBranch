#include "tree.h"

#include "blob.h"
#include "libgit/lgobject.h"
#include "libgit/lgoid.h"
#include "libgit/lgrepository.h"
#include "libgit/lgtree.h"

struct TreePrivate {
        LGRepositoryPtr repo;
        LGTreePtr tree;
};

Tree::~Tree() {
    delete d;
}

BlobPtr Tree::blobForPath(QString path) {
    auto object = d->tree->objectForPath(d->repo, path);
    if (!object) return nullptr;

    auto oid = object->oid();
    return Blob::blobForLgBlob(d->repo->lookupBlob(oid));
}

QString Tree::treeHash() {
    return d->tree->oid()->toHex();
}

TreePtr Tree::treeForLgTree(LGRepositoryPtr repo, LGTreePtr tree) {
    Tree* t = new Tree();
    t->d->tree = tree;
    t->d->repo = repo;
    return t->sharedFromThis();
}

LGTreePtr Tree::gitTree() {
    return d->tree;
}

Tree::Tree(QObject* parent) :
    QObject{parent} {
    d = new TreePrivate;
}
