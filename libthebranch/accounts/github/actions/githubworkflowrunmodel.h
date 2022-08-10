#ifndef GITHUBWORKFLOWRUNMODEL_H
#define GITHUBWORKFLOWRUNMODEL_H

#include "../githubasyncgeneratormodel.h"
#include "githubworkflowjob.h"
#include <QAbstractListModel>

class GitHubWorkflowRun;
typedef QSharedPointer<GitHubWorkflowRun> GitHubWorkflowRunPtr;

struct GitHubWorkflowRunModelPrivate;
class GitHubWorkflowRunModel : public QAbstractListModel,
                               public GitHubAsyncGeneratorModelEngine<GitHubWorkflowJob> {
        Q_OBJECT
        GENERATOR_ENGINE

    public:
        explicit GitHubWorkflowRunModel(GitHubWorkflowRunPtr workflowRun, QObject* parent = nullptr);
        ~GitHubWorkflowRunModel();

        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    private:
        GitHubWorkflowRunModelPrivate* d;

        // GitHubAsyncGeneratorModelEngine interface
    protected:
        QCoro::AsyncGenerator<GitHubWorkflowJobPtr> createGenerator() override;
};

#endif // GITHUBWORKFLOWRUNMODEL_H
