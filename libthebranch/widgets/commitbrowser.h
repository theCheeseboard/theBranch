#ifndef COMMITBROWSER_H
#define COMMITBROWSER_H

#include "objects/forward_declares.h"
#include <QListView>

struct CommitBrowserPrivate;
class CommitBrowser : public QListView {
        Q_OBJECT
    public:
        explicit CommitBrowser(QWidget* parent = nullptr);
        ~CommitBrowser();

        void setRepository(RepositoryPtr repo);
        void setStartBranch(BranchPtr branch);

    signals:

    private:
        CommitBrowserPrivate* d;

        void reloadData();

        // QWidget interface
    protected:
        void contextMenuEvent(QContextMenuEvent* event);
};

#endif // COMMITBROWSER_H
