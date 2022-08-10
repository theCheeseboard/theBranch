#ifndef GITHUBWORKFLOWJOBBROWSER_H
#define GITHUBWORKFLOWJOBBROWSER_H

#include <QWidget>

class GitHubWorkflowJob;
typedef QSharedPointer<GitHubWorkflowJob> GitHubWorkflowJobPtr;

namespace Ui {
    class GitHubWorkflowJobBrowser;
}

struct GitHubWorkflowJobBrowserPrivate;
class GitHubWorkflowJobBrowser : public QWidget {
        Q_OBJECT

    public:
        explicit GitHubWorkflowJobBrowser(GitHubWorkflowJobPtr workflowJob, QWidget* parent = nullptr);
        ~GitHubWorkflowJobBrowser();

    private:
        Ui::GitHubWorkflowJobBrowser* ui;
        GitHubWorkflowJobBrowserPrivate* d;
};

#endif // GITHUBWORKFLOWJOBBROWSER_H
