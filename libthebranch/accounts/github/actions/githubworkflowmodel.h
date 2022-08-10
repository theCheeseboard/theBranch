#ifndef GITHUBWORKFLOWMODEL_H
#define GITHUBWORKFLOWMODEL_H

#include "../githubasyncgeneratormodel.h"
#include "githubworkflowrun.h"
#include <QAbstractListModel>

class GitHubWorkflow;
typedef QSharedPointer<GitHubWorkflow> GitHubWorkflowPtr;

struct GitHubWorkflowModelPrivate;
class GitHubWorkflowModel : public QAbstractListModel,
                            public GitHubAsyncGeneratorModelEngine<GitHubWorkflowRun> {
        Q_OBJECT
        GENERATOR_ENGINE

    public:
        explicit GitHubWorkflowModel(GitHubWorkflowPtr workflow, QObject* parent = nullptr);
        ~GitHubWorkflowModel();

        enum Roles {
            WorkflowRunRole = Qt::UserRole
        };

    private:
        GitHubWorkflowModelPrivate* d;

        // QAbstractItemModel interface
    public:
        QVariant data(const QModelIndex& index, int role) const override;

        // GitHubAsyncGeneratorModelEngine interface
    protected:
        QCoro::AsyncGenerator<GitHubWorkflowRunPtr> createGenerator() override;
};

#endif // GITHUBWORKFLOWMODEL_H
