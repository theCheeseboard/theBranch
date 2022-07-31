#ifndef GITHUBISSUEBROWSER_H
#define GITHUBISSUEBROWSER_H

#include "objects/forward_declares.h"
#include <QWidget>

namespace Ui {
    class GitHubIssueBrowser;
}

class GitHubAccount;
struct GitHubIssueBrowserPrivate;
class GitHubIssueBrowser : public QWidget {
        Q_OBJECT

    public:
        explicit GitHubIssueBrowser(GitHubAccount* account, RemotePtr remote, bool isPr, QWidget* parent = nullptr);
        ~GitHubIssueBrowser();

    private:
        Ui::GitHubIssueBrowser* ui;
        GitHubIssueBrowserPrivate* d;
};

#endif // GITHUBISSUEBROWSER_H
