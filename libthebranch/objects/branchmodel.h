#ifndef BRANCHMODEL_H
#define BRANCHMODEL_H

#include "forward_declares.h"
#include <QAbstractListModel>

struct BranchModelPrivate;
class BranchModel : public QAbstractListModel {
        Q_OBJECT

    public:
        explicit BranchModel(QObject* parent = nullptr);
        ~BranchModel();

        enum BranchModelRole {
            Branch = Qt::UserRole
        };

        // Basic functionality:
        int rowCount(const QModelIndex& parent = QModelIndex()) const override;

        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

        void setRepository(RepositoryPtr repo);
        void reloadData();
        void setBranchFlags(THEBRANCH::ListBranchFlags flags);

    private:
        BranchModelPrivate* d;
};

#endif // BRANCHMODEL_H
