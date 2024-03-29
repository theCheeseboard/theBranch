#ifndef GITHUBITEM_H
#define GITHUBITEM_H

#include "objects/forward_declares.h"
#include <QObject>
#include <concepts>

class GitHubAccount;
class QMenu;
struct GitHubItemPrivate;
class GitHubItem : public QObject {
        Q_OBJECT
    public:
        explicit GitHubItem(GitHubAccount* account, RemotePtr remote);
        ~GitHubItem();

        GitHubAccount* account();
        RemotePtr remote();
        QString nodeId();
        QUrl htmlUrl();

        virtual void update(QJsonObject data);
        virtual void contextMenu(QMenu* menu);
        virtual QCoro::Task<> fetchLatest() = 0;
        virtual QWidget* widget();

    signals:
        void updated();

    private:
        GitHubItemPrivate* d;
};

typedef QSharedPointer<GitHubItem> GitHubItemPtr;

template<typename T>
concept IsGithubItem = std::is_base_of<GitHubItem, T>::value;

#endif // GITHUBITEM_H
