#ifndef REVERTSNAPIN_H
#define REVERTSNAPIN_H

#include "libthebranch_global.h"
#include "objects/forward_declares.h"
#include "snapin.h"
#include <QWidget>

namespace Ui {
    class RevertSnapIn;
}

struct RevertSnapInPrivate;
class LIBTHEBRANCH_EXPORT RevertSnapIn : public SnapIn {
        Q_OBJECT

    public:
        explicit RevertSnapIn(RevertPtr revert, QWidget* parent = nullptr);
        ~RevertSnapIn();

    private slots:
        void on_revertButton_clicked();

        void on_titleLabel_backButtonClicked();

        void on_revertUnavailableButton_clicked();

        void on_titleLabel_2_backButtonClicked();

    private:
        Ui::RevertSnapIn* ui;
        RevertSnapInPrivate* d;
};

#endif // REVERTSNAPIN_H
