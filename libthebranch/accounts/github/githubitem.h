#ifndef GITHUBITEM_H
#define GITHUBITEM_H

#include <QObject>

struct GitHubItemPrivate;
class GitHubItem : public QObject {
        Q_OBJECT
    public:
        explicit GitHubItem();
        ~GitHubItem();

        QString nodeId();

        virtual void update(QJsonObject data);

    signals:
        void updated();

    private:
        GitHubItemPrivate* d;
};

typedef QSharedPointer<GitHubItem> GitHubItemPtr;

#endif // GITHUBITEM_H
