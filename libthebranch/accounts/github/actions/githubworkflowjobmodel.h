#ifndef GITHUBWORKFLOWJOBMODEL_H
#define GITHUBWORKFLOWJOBMODEL_H

#include <QAbstractListModel>

class GitHubWorkflowJob;
typedef QSharedPointer<GitHubWorkflowJob> GitHubWorkflowJobPtr;

struct GitHubWorkflowJobModelPrivate;
class GitHubWorkflowJobModel : public QAbstractListModel {
        Q_OBJECT

    public:
        explicit GitHubWorkflowJobModel(GitHubWorkflowJobPtr workflowJob, QObject* parent = nullptr);
        ~GitHubWorkflowJobModel();

        // Basic functionality:
        int rowCount(const QModelIndex& parent = QModelIndex()) const override;

        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    private:
        GitHubWorkflowJobModelPrivate* d;
};

#endif // GITHUBWORKFLOWJOBMODEL_H
