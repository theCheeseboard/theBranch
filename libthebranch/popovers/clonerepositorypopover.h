#ifndef CLONEREPOSITORYPOPOVER_H
#define CLONEREPOSITORYPOPOVER_H

#include "objects/forward_declares.h"
#include <QWidget>
#include "libthebranch_global.h"

namespace Ui {
    class CloneRepositoryPopover;
}

class Repository;
class LIBTHEBRANCH_EXPORT CloneRepositoryPopover : public QWidget {
        Q_OBJECT

    public:
        explicit CloneRepositoryPopover(QWidget* parent = nullptr);
        ~CloneRepositoryPopover();

    signals:
        void done();
        void openRepository(RepositoryPtr repository);

    private slots:
        void on_titleLabel_backButtonClicked();

        void on_cloneButton_clicked();

        void on_browseButton_clicked();

    private:
        Ui::CloneRepositoryPopover* ui;
};

#endif // CLONEREPOSITORYPOPOVER_H
