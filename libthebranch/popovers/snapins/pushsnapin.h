#ifndef PUSHSNAPIN_H
#define PUSHSNAPIN_H

#include "libthebranch_global.h"
#include "objects/forward_declares.h"
#include "snapin.h"

namespace Ui {
    class PushSnapIn;
}

struct PushSnapInPrivate;
class LIBTHEBRANCH_EXPORT PushSnapIn : public SnapIn {
        Q_OBJECT

    public:
        explicit PushSnapIn(RepositoryPtr repo, QWidget* parent = nullptr);
        ~PushSnapIn();

    private slots:
        void on_titleLabel_backButtonClicked();

        QCoro::Task<> on_pushButton_clicked();

        void on_branchBox_currentIndexChanged(int index);

        void on_titleLabel_2_backButtonClicked();

        void on_pullButton_clicked();

        void on_titleLabel_3_backButtonClicked();

        void on_forcePushButton_clicked();

        void on_pushFailedForcePushButton_clicked();

        void on_doForcePushButton_clicked();

        void on_stackedWidget_switchingFrame(int index);

    private:
        Ui::PushSnapIn* ui;
        PushSnapInPrivate* d;

        void updateUpstreamBox();
        void updatePushButton();

        void prepareForcePush();
        QCoro::Task<> doPush(bool force);

        // SnapIn interface
    public:
        void snapinShown();

        // SnapIn interface
    public:
        tTouchBar* touchBar();
};

#endif // PUSHSNAPIN_H
