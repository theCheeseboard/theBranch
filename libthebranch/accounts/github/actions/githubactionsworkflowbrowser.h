#ifndef GITHUBACTIONSWORKFLOWBROWSER_H
#define GITHUBACTIONSWORKFLOWBROWSER_H

#include <QWidget>

class GitHubWorkflow;
class GitHubWorkflowRun;

typedef QSharedPointer<GitHubWorkflow> GitHubWorkflowPtr;
typedef QSharedPointer<GitHubWorkflowRun> GitHubWorkflowRunPtr;

namespace Ui {
    class GitHubActionsWorkflowBrowser;
}

struct GitHubActionsWorkflowBrowserPrivate;
class GitHubActionsWorkflowBrowser : public QWidget {
        Q_OBJECT

    public:
        explicit GitHubActionsWorkflowBrowser(GitHubWorkflowPtr workflow, QWidget* parent = nullptr);
        ~GitHubActionsWorkflowBrowser();

        void showWorkflowRun(GitHubWorkflowRunPtr workflowRun);

    signals:
        void goBack();

    private slots:
        void on_titleLabel_backButtonClicked();

        void on_listView_clicked(const QModelIndex& index);

    private:
        Ui::GitHubActionsWorkflowBrowser* ui;
        GitHubActionsWorkflowBrowserPrivate* d;
};

#endif // GITHUBACTIONSWORKFLOWBROWSER_H
