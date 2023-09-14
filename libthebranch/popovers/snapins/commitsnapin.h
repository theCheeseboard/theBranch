#ifndef COMMITSNAPIN_H
#define COMMITSNAPIN_H

#include "libthebranch_global.h"
#include "objects/forward_declares.h"
#include "snapin.h"

namespace Ui {
    class CommitSnapIn;
}

struct CommitSnapInPrivate;
class LIBTHEBRANCH_EXPORT CommitSnapIn : public SnapIn {
        Q_OBJECT

    public:
        explicit CommitSnapIn(RepositoryPtr repository, QWidget* parent = nullptr);
        ~CommitSnapIn();

    private slots:
        void on_titleLabel_backButtonClicked();

        void on_commitButton_clicked();

        void on_titleLabel_2_backButtonClicked();

        void on_commitButton_2_clicked();

        void on_promoteLabel_linkActivated(const QString& link);

        void on_selectAllModifiedCheckbox_stateChanged(int arg1);

        void on_viewUntrackedCheckbox_toggled(bool checked);

        void on_modifiedFilesEdit_customContextMenuRequested(const QPoint& pos);

        void on_stackedWidget_switchingFrame(int index);

    private:
        Ui::CommitSnapIn* ui;
        CommitSnapInPrivate* d;

        void performCommit();
        void updateState();
        void updateSelection();
        tTouchBar* touchBar();
};

#endif // COMMITSNAPIN_H
