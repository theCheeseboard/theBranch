#ifndef BRANCHBROWSER_H
#define BRANCHBROWSER_H

#include "objects/forward_declares.h"
#include <QListView>

struct BranchBrowserPrivate;
class BranchBrowser : public QListView {
        Q_OBJECT
    public:
        explicit BranchBrowser(QWidget* parent = nullptr);
        ~BranchBrowser();

        void setRepository(RepositoryPtr repo);

    signals:

    private:
        BranchBrowserPrivate* d;

        // QWidget interface
    protected:
    QString extracted();
    
    BranchPtr extracted(const QModelIndex &index);
    
    void contextMenuEvent(QContextMenuEvent* event);
};

#endif // BRANCHBROWSER_H
