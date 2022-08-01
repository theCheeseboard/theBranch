#ifndef GITHUBISSUECOMMENTEVENT_H
#define GITHUBISSUECOMMENTEVENT_H

#include "githubissueevent.h"

struct GitHubIssueCommentEventPrivate;
class GitHubIssueCommentEvent : public GitHubIssueEvent {
        Q_OBJECT
    public:
        explicit GitHubIssueCommentEvent(GitHubAccount* account, RemotePtr remote);
        ~GitHubIssueCommentEvent();

        QString body();
        GitHubUserPtr user();

    signals:

    private:
        GitHubIssueCommentEventPrivate* d;

        // GitHubItem interface
    public:
        void update(QJsonObject data);
};

typedef QSharedPointer<GitHubIssueCommentEvent> GitHubIssueCommentEventPtr;

#endif // GITHUBISSUECOMMENTEVENT_H
