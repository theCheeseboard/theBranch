#ifndef STATUSITEMLISTMODEL_H
#define STATUSITEMLISTMODEL_H

#include "diff.h"
#include "repository.h"
#include <QAbstractListModel>
#include <QStyledItemDelegate>
#include <tpaintcalculator.h>

struct StatusItemListModelPrivate;
class StatusItemListModel : public QAbstractListModel {
        Q_OBJECT

    public:
        explicit StatusItemListModel(QObject* parent = nullptr);
        ~StatusItemListModel();

        enum Roles {
            PathRole = Qt::UserRole,
            StatusRole,
            RenamedPathRole,

            BlobOldSide,
            BlobNewSide
        };

        void setStatusItems(QList<Repository::StatusItem> items);
        void setDiffResult(QList<Diff::DiffFile> results);
        void setUserCheckable(bool userCheckable);

        // Basic functionality:
        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

        QModelIndexList checkedItems();

    signals:
        void checkedItemsChanged();

    private:
        StatusItemListModelPrivate* d;

        // QAbstractItemModel interface
    public:
        Qt::ItemFlags flags(const QModelIndex& index) const override;
        bool setData(const QModelIndex& index, const QVariant& value, int role) override;
};

struct StatusItemListFilterViewPrivate;
class StatusItemListFilterView : public QSortFilterProxyModel {
        Q_OBJECT

    public:
        explicit StatusItemListFilterView(QObject* parent = nullptr);
        ~StatusItemListFilterView();

        void setFlagFilters(int flagFilters);

        bool acceptRowWithFlags(int flagFilters, int source_row, const QModelIndex& source_parent) const;

    private:
        StatusItemListFilterViewPrivate* d;

        // QSortFilterProxyModel interface
    protected:
        bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const;
};

class StatusItemListDelegate : public QStyledItemDelegate {
        Q_OBJECT

    public:
        explicit StatusItemListDelegate(QObject* parent = nullptr);

    private:
        tPaintCalculator paintCalculator(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index);

        // QAbstractItemDelegate interface
    public:
        void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
        QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

#endif // STATUSITEMLISTMODEL_H
