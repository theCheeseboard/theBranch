#ifndef GITHUBACTIONSLISTCONTROLLER_H
#define GITHUBACTIONSLISTCONTROLLER_H

#include "objects/forward_declares.h"
#include <QCoroAsyncGenerator>
#include <QObject>

class GitHubAccount;
class QStandardItem;
struct GitHubActionsListControllerPrivate;
class GitHubActionsListController : public QObject {
        Q_OBJECT
    public:
        explicit GitHubActionsListController(GitHubAccount* account, RemotePtr remote, QObject* parent = nullptr);
        ~GitHubActionsListController();

        QStandardItem* rootItem();

    signals:

    private:
        GitHubActionsListControllerPrivate* d;

        QCoro::Task<> updateItems();
};

#endif // GITHUBACTIONSLISTCONTROLLER_H
