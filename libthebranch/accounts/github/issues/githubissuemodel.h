#ifndef GITHUBISSUEMODEL_H
#define GITHUBISSUEMODEL_H

#include "../githubasyncgeneratormodel.h"
#include "githubissue.h"
#include "objects/forward_declares.h"
#include <QAbstractListModel>

class GitHubAccount;
struct GitHubIssueModelPrivate;
class GitHubIssueModel : public QAbstractListModel,
                         public GitHubAsyncGeneratorModelEngine<GitHubIssue> {
        Q_OBJECT
        GENERATOR_ENGINE

    public:
        explicit GitHubIssueModel(GitHubAccount* account, RemotePtr remote, bool isPr, QObject* parent = nullptr);
        ~GitHubIssueModel();

        enum Roles {
            IssueRole = Qt::UserRole
        };

        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    private:
        GitHubIssueModelPrivate* d;

        // GitHubAsyncGeneratorModelEngine interface
    protected:
        QCoro::AsyncGenerator<GitHubIssuePtr> createGenerator() override;
};

#endif // GITHUBISSUEMODEL_H
