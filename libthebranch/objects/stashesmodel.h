#ifndef STASHESMODEL_H
#define STASHESMODEL_H

#include "forward_declares.h"
#include <QAbstractListModel>

struct StashesModelPrivate;
class StashesModel : public QAbstractListModel {
        Q_OBJECT

    public:
        explicit StashesModel(QObject* parent = nullptr);
        ~StashesModel();

        enum StashesModelRole {
            Stash = Qt::UserRole
        };

        // Basic functionality:
        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

        void setRepository(RepositoryPtr repo);
        void reloadData();

    private:
        StashesModelPrivate* d;
};

#endif // STASHESMODEL_H
