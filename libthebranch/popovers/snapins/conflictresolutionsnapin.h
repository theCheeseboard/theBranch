#ifndef CONFLICTRESOLUTIONSNAPIN_H
#define CONFLICTRESOLUTIONSNAPIN_H

#include "libthebranch_global.h"
#include "objects/forward_declares.h"
#include "snapin.h"

namespace Ui {
    class ConflictResolutionSnapIn;
}

struct ConflictResolutionSnapInPrivate;
class LIBTHEBRANCH_EXPORT ConflictResolutionSnapIn : public SnapIn {
        Q_OBJECT

    public:
        explicit ConflictResolutionSnapIn(GitOperationPtr gitOperation, QWidget* parent = nullptr);
        ~ConflictResolutionSnapIn();

        void snapinShown();

    private slots:
        void on_titleLabel_backButtonClicked();

        void on_continueConflictResolutionButton_clicked();

        void on_doAbortButton_clicked();

        void on_completeButton_clicked();

        void on_stackedWidget_switchingFrame(int index);

    private:
        Ui::ConflictResolutionSnapIn* ui;
        ConflictResolutionSnapInPrivate* d;

        void updateConflictResolutionState();

        // SnapIn interface
    public:
        tTouchBar* touchBar();
};

#endif // CONFLICTRESOLUTIONSNAPIN_H
