#ifndef GITHUBISSUE_H
#define GITHUBISSUE_H

#include "../githubitem.h"

struct GitHubIssuePrivate;
class GitHubIssue : public GitHubItem {
        Q_OBJECT
    public:
        explicit GitHubIssue();
        ~GitHubIssue();

        qint64 number();
        QString title();

    signals:

    private:
        GitHubIssuePrivate* d;

        // GitHubItem interface
    public:
        void update(QJsonObject data);
};

typedef QSharedPointer<GitHubIssue> GitHubIssuePtr;

#endif // GITHUBISSUE_H
