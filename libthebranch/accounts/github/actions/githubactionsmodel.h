#ifndef GITHUBACTIONSMODEL_H
#define GITHUBACTIONSMODEL_H

#include "objects/forward_declares.h"
#include <QAbstractListModel>
#include <QCoroTask>

class GitHubAccount;
struct GitHubActionsModelPrivate;
class GitHubActionsModel : public QAbstractListModel {
        Q_OBJECT

    public:
        explicit GitHubActionsModel(GitHubAccount* account, RemotePtr remote, QObject* parent = nullptr);
        ~GitHubActionsModel();

        // Basic functionality:
        int rowCount(const QModelIndex& parent = QModelIndex()) const override;

        // Fetch data dynamically:
        bool hasChildren(const QModelIndex& parent = QModelIndex()) const override;

        bool canFetchMore(const QModelIndex& parent) const override;
        void fetchMore(const QModelIndex& parent) override;

        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    private:
        GitHubActionsModelPrivate* d;

        QCoro::Task<> startFetch();
        QCoro::Task<> fetchNext();
};

#endif // GITHUBACTIONSMODEL_H
