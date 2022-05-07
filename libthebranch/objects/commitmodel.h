#ifndef COMMITMODEL_H
#define COMMITMODEL_H

#include "forward_declares.h"
#include <QAbstractListModel>
#include <QStyledItemDelegate>
#include <tpaintcalculator.h>

struct CommitModelPrivate;
class CommitModel : public QAbstractListModel {
        Q_OBJECT

    public:
        explicit CommitModel(QObject* parent = nullptr);
        ~CommitModel();

        enum CommitModelRole {
            CommitHash = Qt::UserRole,
            CommitMessage,
            AuthorName,
            Commit
        };

        void setStartPoint(QString startPoint);
        void setRepository(RepositoryPtr repo);
        void reloadData();

        // Basic functionality:
        int rowCount(const QModelIndex& parent = QModelIndex()) const override;

        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    private:
        CommitModelPrivate* d;
};

class CommitDelegate : public QStyledItemDelegate {
        Q_OBJECT

    public:
        explicit CommitDelegate(QObject* parent = nullptr);

        // QAbstractItemDelegate interface
    public:
        void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
        QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;

    private:
        tPaintCalculator paintCalculator(const QStyleOptionViewItem& option, const QModelIndex& index, QPainter* painter = nullptr) const;
};

#endif // COMMITMODEL_H
