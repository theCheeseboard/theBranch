#ifndef GITHUBISSUE_H
#define GITHUBISSUE_H

#include "../githubitem.h"
#include <QCoroAsyncGenerator>

class GitHubIssueEvent;
typedef QSharedPointer<GitHubIssueEvent> GitHubIssueEventPtr;

struct GitHubIssuePrivate;
class GitHubIssue : public GitHubItem {
        Q_OBJECT
    public:
        explicit GitHubIssue(GitHubAccount* account, RemotePtr remote);
        ~GitHubIssue();

        enum class State {
            Open,
            Closed,
            Merged
        };

        qint64 number();
        QString title();
        QString body();
        State state();

        QCoro::AsyncGenerator<GitHubIssueEventPtr> listIssueEvents();

    signals:

    private:
        GitHubIssuePrivate* d;

        // GitHubItem interface
    public:
        void update(QJsonObject data);
        void contextMenu(QMenu* menu);
        QCoro::Task<> fetchLatest();
};

typedef QSharedPointer<GitHubIssue> GitHubIssuePtr;

#endif // GITHUBISSUE_H
