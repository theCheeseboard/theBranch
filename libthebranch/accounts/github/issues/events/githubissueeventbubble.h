#ifndef GITHUBISSUEEVENTBUBBLE_H
#define GITHUBISSUEEVENTBUBBLE_H

#include <QWidget>

namespace Ui {
    class GitHubIssueEventBubble;
}

class GitHubIssueEvent;
typedef QSharedPointer<GitHubIssueEvent> GitHubIssueEventPtr;

struct GitHubIssueEventBubblePrivate;
class GitHubIssueEventBubble : public QWidget {
        Q_OBJECT

    public:
        explicit GitHubIssueEventBubble(GitHubIssueEventPtr item, QWidget* parent = nullptr);
        ~GitHubIssueEventBubble();

    private:
        Ui::GitHubIssueEventBubble* ui;
        GitHubIssueEventBubblePrivate* d;

        void updateData();
};

#endif // GITHUBISSUEEVENTBUBBLE_H
