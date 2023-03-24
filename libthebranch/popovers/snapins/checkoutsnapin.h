#ifndef CHECKOUTSNAPIN_H
#define CHECKOUTSNAPIN_H

#include "libthebranch_global.h"
#include "objects/forward_declares.h"
#include "snapin.h"

namespace Ui {
    class CheckoutSnapIn;
}

struct CheckoutSnapInPrivate;
class LIBTHEBRANCH_EXPORT CheckoutSnapIn : public SnapIn {
        Q_OBJECT

    public:
        explicit CheckoutSnapIn(RepositoryPtr repository, QWidget* parent = nullptr);
        ~CheckoutSnapIn();

    private slots:
        void on_titleLabel_backButtonClicked();

        void on_checkoutButton_clicked();

        void on_checkoutCommitButton_toggled(bool checked);

        void on_checkoutBranchButton_toggled(bool checked);

    private:
        Ui::CheckoutSnapIn* ui;
        CheckoutSnapInPrivate* d;

        void updateWidgets();
};

#endif // CHECKOUTSNAPIN_H
