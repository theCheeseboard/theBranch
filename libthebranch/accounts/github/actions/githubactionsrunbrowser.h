#ifndef GITHUBACTIONSRUNBROWSER_H
#define GITHUBACTIONSRUNBROWSER_H

#include <QWidget>

class GitHubWorkflowRun;
typedef QSharedPointer<GitHubWorkflowRun> GitHubWorkflowRunPtr;

namespace Ui {
    class GitHubActionsRunBrowser;
}

struct GitHubActionsRunBrowserPrivate;
class GitHubActionsRunBrowser : public QWidget {
        Q_OBJECT

    public:
        explicit GitHubActionsRunBrowser(GitHubWorkflowRunPtr workflowRun, QWidget* parent = nullptr);
        ~GitHubActionsRunBrowser();

    signals:
        void goBack();

    private slots:
        void on_titleLabel_backButtonClicked();

    private:
        Ui::GitHubActionsRunBrowser* ui;
        GitHubActionsRunBrowserPrivate* d;
};

#endif // GITHUBACTIONSRUNBROWSER_H
