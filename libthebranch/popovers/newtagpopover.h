#ifndef NEWTAGPOPOVER_H
#define NEWTAGPOPOVER_H

#include "objects/forward_declares.h"
#include <QWidget>

namespace Ui {
    class NewTagPopover;
}

struct NewTagPopoverPrivate;
class NewTagPopover : public QWidget {
        Q_OBJECT

    public:
        explicit NewTagPopover(RepositoryPtr repo, ICommitResolvablePtr commit, QWidget* parent = nullptr);
        ~NewTagPopover();

    private slots:
        void on_tagButton_clicked();

        void on_titleLabel_backButtonClicked();

        void on_nameBox_textChanged(const QString& arg1);

    signals:
        void done();

    private:
        Ui::NewTagPopover* ui;
        NewTagPopoverPrivate* d;
};

#endif // NEWTAGPOPOVER_H
