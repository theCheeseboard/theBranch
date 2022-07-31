#ifndef GITHUBISSUEMODEL_H
#define GITHUBISSUEMODEL_H

#include "objects/forward_declares.h"
#include <QAbstractListModel>

class GitHubAccount;
struct GitHubIssueModelPrivate;
class GitHubIssueModel : public QAbstractListModel {
        Q_OBJECT

    public:
        explicit GitHubIssueModel(GitHubAccount* account, RemotePtr remote, bool isPr, QObject* parent = nullptr);
        ~GitHubIssueModel();

        enum Roles {
            IssueRole = Qt::UserRole
        };

        // Basic functionality:
        int rowCount(const QModelIndex& parent = QModelIndex()) const override;

        // Fetch data dynamically:
        bool hasChildren(const QModelIndex& parent = QModelIndex()) const override;

        bool canFetchMore(const QModelIndex& parent) const override;
        void fetchMore(const QModelIndex& parent) override;

        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    private:
        GitHubIssueModelPrivate* d;

        QCoro::Task<> startFetch();
        QCoro::Task<> fetchNext();
};

#endif // GITHUBISSUEMODEL_H
