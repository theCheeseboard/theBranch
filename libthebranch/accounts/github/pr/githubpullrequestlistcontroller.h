#ifndef GITHUBPULLREQUESTLISTCONTROLLER_H
#define GITHUBPULLREQUESTLISTCONTROLLER_H

#include "objects/forward_declares.h"
#include <QObject>

class QStandardItem;
class GitHubAccount;
struct GitHubPullRequestListControllerPrivate;
class GitHubPullRequestListController : public QObject {
        Q_OBJECT
    public:
        explicit GitHubPullRequestListController(GitHubAccount* account, RemotePtr remote, QObject* parent = nullptr);
        ~GitHubPullRequestListController();

        QStandardItem* rootItem();

    signals:

    private:
        GitHubPullRequestListControllerPrivate* d;

        QCoro::Task<> updateItems();
};

#endif // GITHUBPULLREQUESTLISTCONTROLLER_H
