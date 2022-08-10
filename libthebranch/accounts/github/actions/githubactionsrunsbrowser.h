#ifndef GITHUBACTIONSRUNSBROWSER_H
#define GITHUBACTIONSRUNSBROWSER_H

#include <QWidget>

class GitHubWorkflow;
typedef QSharedPointer<GitHubWorkflow> GitHubWorkflowPtr;

namespace Ui {
    class GitHubActionsRunsBrowser;
}

struct GitHubActionsRunsBrowserPrivate;
class GitHubActionsRunsBrowser : public QWidget {
        Q_OBJECT

    public:
        explicit GitHubActionsRunsBrowser(GitHubWorkflowPtr workflow, QWidget* parent = nullptr);
        ~GitHubActionsRunsBrowser();

    signals:
        void goBack();

    private slots:
        void on_titleLabel_backButtonClicked();

    private:
        Ui::GitHubActionsRunsBrowser* ui;
        GitHubActionsRunsBrowserPrivate* d;
};

#endif // GITHUBACTIONSRUNSBROWSER_H
