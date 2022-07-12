#ifndef CHECKOUTSNAPIN_H
#define CHECKOUTSNAPIN_H

#include "objects/forward_declares.h"
#include "snapin.h"
#include "libthebranch_global.h"

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

    private:
        Ui::CheckoutSnapIn* ui;
        CheckoutSnapInPrivate* d;
};

#endif // CHECKOUTSNAPIN_H
