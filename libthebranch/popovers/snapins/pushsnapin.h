#ifndef PUSHSNAPIN_H
#define PUSHSNAPIN_H

#include "libthebranch_global.h"
#include "objects/forward_declares.h"
#include "snapin.h"
#include <QCoroTask>

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

    private:
        Ui::PushSnapIn* ui;
        PushSnapInPrivate* d;

        void updateUpstreamBox();
};

#endif // PUSHSNAPIN_H
