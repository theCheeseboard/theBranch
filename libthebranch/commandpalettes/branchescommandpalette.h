#ifndef BRANCHESCOMMANDPALETTE_H
#define BRANCHESCOMMANDPALETTE_H

#include <tcommandpalette/tcommandpalettescope.h>

class Repository;
struct BranchesCommandPalettePrivate;
class BranchesCommandPalette : public tCommandPaletteScope {
        Q_OBJECT
    public:
        explicit BranchesCommandPalette(Repository* repository);
        ~BranchesCommandPalette();

    private:
        BranchesCommandPalettePrivate* d;

        // QAbstractItemModel interface
    public:
        int rowCount(const QModelIndex& parent) const;
        QVariant data(const QModelIndex& index, int role) const;

        // tCommandPaletteScope interface
    public:
        QString displayName();
        void filter(QString filter);
        void activate(QModelIndex index);
};

#endif // BRANCHESCOMMANDPALETTE_H
