#ifndef CLONEREPOSITORYSNAPIN_H
#define CLONEREPOSITORYSNAPIN_H

#include "libthebranch_global.h"
#include "objects/forward_declares.h"
#include "snapin.h"

namespace Ui {
    class CloneRepositorySnapIn;
}

struct CloneRepositorySnapInPrivate;
class Repository;
class LIBTHEBRANCH_EXPORT CloneRepositorySnapIn : public SnapIn {
        Q_OBJECT

    public:
        explicit CloneRepositorySnapIn(QWidget* parent = nullptr);
        ~CloneRepositorySnapIn();

    signals:
        void openRepository(RepositoryPtr repository);

    private slots:
        void on_titleLabel_backButtonClicked();

        QCoro::Task<> on_cloneButton_clicked();

        void on_browseButton_clicked();

        void on_titleLabel_2_backButtonClicked();

        void on_cloneButton_2_clicked();

    private:
        Ui::CloneRepositorySnapIn* ui;
        CloneRepositorySnapInPrivate* d;
};

#endif // CLONEREPOSITORYSNAPIN_H
