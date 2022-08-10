#ifndef GITHUBACTIONSMODEL_H
#define GITHUBACTIONSMODEL_H

#include "../githubasyncgeneratormodel.h"
#include "githubworkflow.h"
#include "objects/forward_declares.h"
#include <QAbstractListModel>
#include <QCoroTask>

class GitHubAccount;
struct GitHubActionsModelPrivate;
class GitHubActionsModel : public QAbstractListModel,
                           public GitHubAsyncGeneratorModelEngine<GitHubWorkflow> {
        Q_OBJECT
        GENERATOR_ENGINE

    public:
        explicit GitHubActionsModel(GitHubAccount* account, RemotePtr remote, QObject* parent = nullptr);
        ~GitHubActionsModel();

        enum Roles {
            WorkflowRole = Qt::UserRole
        };

        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    private:
        GitHubActionsModelPrivate* d;

        // GitHubAsyncGeneratorModelEngine interface
    protected:
        QCoro::AsyncGenerator<GitHubWorkflowPtr> createGenerator() override;
};

#endif // GITHUBACTIONSMODEL_H
