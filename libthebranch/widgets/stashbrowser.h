#ifndef STASHBROWSER_H
#define STASHBROWSER_H

#include "libthebranch_global.h"
#include "objects/forward_declares.h"
#include <QListView>

struct StashBrowserPrivate;
class LIBTHEBRANCH_EXPORT StashBrowser : public QListView {
        Q_OBJECT
    public:
        explicit StashBrowser(QWidget* parent = nullptr);
        ~StashBrowser();

        void setRepository(RepositoryPtr repo);

    signals:

    private:
        StashBrowserPrivate* d;

        // QWidget interface
protected:
        void contextMenuEvent(QContextMenuEvent *event);
};

#endif // STASHBROWSER_H
