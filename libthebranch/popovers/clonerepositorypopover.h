#ifndef CLONEREPOSITORYPOPOVER_H
#define CLONEREPOSITORYPOPOVER_H

#include <QWidget>

namespace Ui {
    class CloneRepositoryPopover;
}

class Repository;
class CloneRepositoryPopover : public QWidget {
        Q_OBJECT

    public:
        explicit CloneRepositoryPopover(QWidget* parent = nullptr);
        ~CloneRepositoryPopover();

    signals:
        void done();
        void openRepository(Repository* repository);

    private slots:
        void on_titleLabel_backButtonClicked();

        void on_cloneButton_clicked();

        void on_browseButton_clicked();

    private:
        Ui::CloneRepositoryPopover* ui;
};

#endif // CLONEREPOSITORYPOPOVER_H
