#ifndef REPOSITORYSTATUSBAR_H
#define REPOSITORYSTATUSBAR_H

#include "objects/forward_declares.h"
#include <QWidget>

namespace Ui {
    class RepositoryStatusBar;
}

struct RepositoryStatusBarPrivate;
class RepositoryStatusBar : public QWidget {
        Q_OBJECT

    public:
        explicit RepositoryStatusBar(QWidget* parent = nullptr);
        ~RepositoryStatusBar();

        void setRepository(RepositoryPtr repository);

    private:
        Ui::RepositoryStatusBar* ui;
        RepositoryStatusBarPrivate* d;

        void updateRepository();
};

#endif // REPOSITORYSTATUSBAR_H
