#ifndef GITHUBISSUE_H
#define GITHUBISSUE_H

#include "../githubitem.h"
#include <QCoroAsyncGenerator>

class GitHubUser;
class GitHubIssueEvent;
typedef QSharedPointer<GitHubIssueEvent> GitHubIssueEventPtr;
typedef QSharedPointer<GitHubUser> GitHubUserPtr;

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
        GitHubUserPtr user();

        QCoro::AsyncGenerator<GitHubIssueEventPtr> listIssueEvents();
        QCoro::Task<> postComment(QString comment);
        QCoro::Task<> setState(State state);

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
