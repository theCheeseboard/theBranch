#ifndef GITHUBITEM_H
#define GITHUBITEM_H

#include <QObject>

class QMenu;
struct GitHubItemPrivate;
class GitHubItem : public QObject {
        Q_OBJECT
    public:
        explicit GitHubItem();
        ~GitHubItem();

        QString nodeId();
        QUrl htmlUrl();

        virtual void update(QJsonObject data);
        virtual void contextMenu(QMenu* menu);
        virtual QWidget* widget();

    signals:
        void updated();

    private:
        GitHubItemPrivate* d;
};

typedef QSharedPointer<GitHubItem> GitHubItemPtr;

#endif // GITHUBITEM_H
