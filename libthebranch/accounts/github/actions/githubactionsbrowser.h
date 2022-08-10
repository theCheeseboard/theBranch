#ifndef GITHUBACTIONSBROWSER_H
#define GITHUBACTIONSBROWSER_H

#include "objects/forward_declares.h"
#include <QWidget>

class GitHubWorkflow;
typedef QSharedPointer<GitHubWorkflow> GitHubWorkflowPtr;

namespace Ui {
    class GitHubActionsBrowser;
}

class GitHubAccount;
struct GitHubActionsBrowserPrivate;
class GitHubActionsBrowser : public QWidget {
        Q_OBJECT

    public:
        explicit GitHubActionsBrowser(GitHubAccount* account, RemotePtr remote, QWidget* parent = nullptr);
        ~GitHubActionsBrowser();

        void showWorkflow(GitHubWorkflowPtr workflow);

    private slots:
        void on_listView_clicked(const QModelIndex& index);

    private:
        Ui::GitHubActionsBrowser* ui;
        GitHubActionsBrowserPrivate* d;
};

#endif // GITHUBACTIONSBROWSER_H
