#ifndef GITHUBACTIONSRUNSBROWSER_H
#define GITHUBACTIONSRUNSBROWSER_H

#include <QWidget>

namespace Ui {
class GitHubActionsRunsBrowser;
}

class GitHubActionsRunsBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit GitHubActionsRunsBrowser(QWidget *parent = nullptr);
    ~GitHubActionsRunsBrowser();

private:
    Ui::GitHubActionsRunsBrowser *ui;
};

#endif // GITHUBACTIONSRUNSBROWSER_H
