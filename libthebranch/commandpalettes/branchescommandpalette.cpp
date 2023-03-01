#include "branchescommandpalette.h"

#include "objects/repository.h"

struct BranchesCommandPalettePrivate {
        Repository* repository;
        QList<BranchPtr> shownBranches;

        QString filter;
};

BranchesCommandPalette::BranchesCommandPalette(Repository* repository) :
    tCommandPaletteScope{repository} {
    d = new BranchesCommandPalettePrivate();
    d->repository = repository;

    connect(d->repository, &Repository::repositoryUpdated, this, [this] {
        filter(d->filter);
    });
}

BranchesCommandPalette::~BranchesCommandPalette() {
    delete d;
}

int BranchesCommandPalette::rowCount(const QModelIndex& parent) const {
    return d->shownBranches.count();
}

QVariant BranchesCommandPalette::data(const QModelIndex& index, int role) const {
    auto branch = d->shownBranches.at(index.row());
    switch (role) {
        case Qt::DisplayRole:
            return branch->name();
    }

    return QVariant();
}

QString BranchesCommandPalette::displayName() {
    return tr("Branches");
}

void BranchesCommandPalette::filter(QString filter) {
    beginResetModel();
    d->shownBranches.clear();
    for (auto branch : d->repository->branches(THEBRANCH::AllBranches)) {
        if (branch->name().toLower().contains(filter.toLower()) || filter.isEmpty()) {
            d->shownBranches.append(branch);
        }
    }
    d->filter = filter;
    endResetModel();
}

void BranchesCommandPalette::activate(QModelIndex index) {
}
