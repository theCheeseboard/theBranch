#ifndef STASHSAVESNAPIN_H
#define STASHSAVESNAPIN_H

#include "libthebranch_global.h"
#include "objects/forward_declares.h"
#include "snapin.h"

namespace Ui {
    class StashSaveSnapIn;
}

struct StashSaveSnapInPrivate;
class LIBTHEBRANCH_EXPORT StashSaveSnapIn : public SnapIn {
        Q_OBJECT

    public:
        explicit StashSaveSnapIn(RepositoryPtr repo, QWidget* parent = nullptr);
        ~StashSaveSnapIn();

    private slots:
        void on_titleLabel_backButtonClicked();

        QCoro::Task<> on_stashButton_clicked();

    private:
        Ui::StashSaveSnapIn* ui;
        StashSaveSnapInPrivate* d;
};

#endif // STASHSAVESNAPIN_H
