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

    private slots:
        void on_descriptionLabel_linkActivated(const QString& link);

    private:
        Ui::GitHubIssueEventBubble* ui;
        GitHubIssueEventBubblePrivate* d;

        static QString embolden(QString text);
        static QString link(QString text, QUrl link);
        static QString commit(QString id);

        void updateData();
};

#endif // GITHUBISSUEEVENTBUBBLE_H
