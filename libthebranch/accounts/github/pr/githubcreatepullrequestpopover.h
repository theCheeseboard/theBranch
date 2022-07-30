#ifndef GITHUBCREATEPULLREQUESTPOPOVER_H
#define GITHUBCREATEPULLREQUESTPOPOVER_H

#include "objects/forward_declares.h"
#include <QWidget>

namespace Ui {
    class GitHubCreatePullRequestPopover;
}

class GitHubAccount;
struct GitHubCreatePullRequestPopoverPrivate;
class GitHubCreatePullRequestPopover : public QWidget {
        Q_OBJECT

    public:
        explicit GitHubCreatePullRequestPopover(GitHubAccount* account, BranchPtr from, BranchPtr to, RemotePtr toRemote, QWidget* parent = nullptr);
        ~GitHubCreatePullRequestPopover();

    private slots:
        void on_titleLabel_backButtonClicked();

        QCoro::Task<> on_createPrButton_clicked();

    signals:
        void done();

    private:
        Ui::GitHubCreatePullRequestPopover* ui;
        GitHubCreatePullRequestPopoverPrivate* d;
};

#endif // GITHUBCREATEPULLREQUESTPOPOVER_H
