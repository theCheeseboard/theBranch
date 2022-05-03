#ifndef REPOSITORYBROWSER_H
#define REPOSITORYBROWSER_H

#include "objects/forward_declares.h"
#include <QWidget>

namespace Ui {
    class RepositoryBrowser;
}

struct RepositoryBrowserPrivate;
class RepositoryBrowser : public QWidget {
        Q_OBJECT

    public:
        explicit RepositoryBrowser(QWidget* parent = nullptr);
        ~RepositoryBrowser();

        void setRepository(RepositoryPtr repository);
        RepositoryPtr repository();

        QString title();

    private slots:
        void on_cloneButton_clicked();

        void on_openRepositoryButton_clicked();

    signals:
        void titleChanged();

    private:
        Ui::RepositoryBrowser* ui;
        RepositoryBrowserPrivate* d;

        void updateRepositoryState();
};

#endif // REPOSITORYBROWSER_H
