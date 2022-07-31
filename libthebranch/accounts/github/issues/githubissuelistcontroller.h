#ifndef GITHUBISSUELISTCONTROLLER_H
#define GITHUBISSUELISTCONTROLLER_H

#include "objects/forward_declares.h"
#include <QCoroAsyncGenerator>
#include <QObject>

class GitHubAccount;
class QStandardItem;
struct GitHubIssueListControllerPrivate;
class GitHubIssueListController : public QObject {
        Q_OBJECT
    public:
        explicit GitHubIssueListController(GitHubAccount* account, RemotePtr remote, QObject* parent = nullptr);
        ~GitHubIssueListController();

        QStandardItem* rootItem();

    signals:

    private:
        GitHubIssueListControllerPrivate* d;

        QCoro::Task<> updateItems();
};

#endif // GITHUBISSUELISTCONTROLLER_H
