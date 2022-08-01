#ifndef GITHUBITEMDATABASE_H
#define GITHUBITEMDATABASE_H

#include "objects/forward_declares.h"
#include <QJsonObject>
#include <QMap>
#include <QObject>
#include <concepts>

class GitHubAccount;
class GitHubItem;

typedef QSharedPointer<GitHubItem> GitHubItemPtr;

template<typename T>
concept IsGithubItem = std::is_base_of<GitHubItem, T>::value;

class GitHubItemDatabase : public QObject {
        Q_OBJECT
    public:
        explicit GitHubItemDatabase(QObject* parent = nullptr);
        ~GitHubItemDatabase();

        GitHubItemPtr item(QString nodeId);
        template<IsGithubItem T> QSharedPointer<T> update(GitHubAccount* account, RemotePtr remote, QJsonObject data) {
            if (!data.contains("node_id")) return nullptr;
            auto nodeId = data.value("node_id").toString();

            QSharedPointer<T> type;
            if (this->items.contains(nodeId)) {
                type = this->items.value(nodeId).objectCast<T>();
            } else {
                type = QSharedPointer<T>(new T(account, remote));
                items.insert(nodeId, type);
            }

            type->update(data);
            return type;
        }

    signals:

    private:
        QMap<QString, GitHubItemPtr> items;
};

#endif // GITHUBITEMDATABASE_H
