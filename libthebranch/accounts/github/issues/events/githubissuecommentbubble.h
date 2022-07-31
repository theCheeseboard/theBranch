#ifndef GITHUBISSUECOMMENTBUBBLE_H
#define GITHUBISSUECOMMENTBUBBLE_H

#include <QWidget>

namespace Ui {
    class GitHubIssueCommentBubble;
}

class GitHubItem;
typedef QSharedPointer<GitHubItem> GitHubItemPtr;

struct GitHubIssueCommentBubblePrivate;
class GitHubIssueCommentBubble : public QWidget {
        Q_OBJECT

    public:
        explicit GitHubIssueCommentBubble(GitHubItemPtr item, QWidget* parent = nullptr);
        ~GitHubIssueCommentBubble();

    private:
        Ui::GitHubIssueCommentBubble* ui;
        GitHubIssueCommentBubblePrivate* d;

        void updateData();
};

#endif // GITHUBISSUECOMMENTBUBBLE_H
