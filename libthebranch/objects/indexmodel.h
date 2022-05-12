#ifndef INDEXMODEL_H
#define INDEXMODEL_H

#include "forward_declares.h"
#include <QAbstractListModel>

struct IndexModelPrivate;
class IndexModel : public QAbstractListModel {
        Q_OBJECT

    public:
        explicit IndexModel(QObject* parent = nullptr);
        ~IndexModel();

        void setIndex(IndexPtr index);
        void reloadData();

        // Basic functionality:
        int rowCount(const QModelIndex& parent = QModelIndex()) const override;

        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    private:
        IndexModelPrivate* d;
};

#endif // INDEXMODEL_H
