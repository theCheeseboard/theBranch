#ifndef GITHUBISSUEBROWSER_H
#define GITHUBISSUEBROWSER_H

#include "objects/forward_declares.h"
#include <QWidget>

namespace Ui {
    class GitHubIssueBrowser;
}

class GitHubIssue;
typedef QSharedPointer<GitHubIssue> GitHubIssuePtr;

class GitHubAccount;
struct GitHubIssueBrowserPrivate;
class GitHubIssueBrowser : public QWidget {
        Q_OBJECT

    public:
        explicit GitHubIssueBrowser(GitHubAccount* account, RemotePtr remote, bool isPr, QWidget* parent = nullptr);
        ~GitHubIssueBrowser();

        void openIssue(GitHubIssuePtr issue);

    private slots:
        void on_titleLabel_2_backButtonClicked();

        void on_listView_clicked(const QModelIndex& index);

    private:
        Ui::GitHubIssueBrowser* ui;
        GitHubIssueBrowserPrivate* d;

        void readCurrentIssue();
        QCoro::Task<> addItemsIfNeeded();
};

#endif // GITHUBISSUEBROWSER_H
