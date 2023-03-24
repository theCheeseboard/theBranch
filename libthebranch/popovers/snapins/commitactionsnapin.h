#ifndef COMMITACTIONSNAPIN_H
#define COMMITACTIONSNAPIN_H

#include "../../libthebranch_global.h"
#include "objects/forward_declares.h"
#include "snapin.h"

namespace Ui {
    class CommitActionSnapIn;
}

struct CommitActionSnapInPrivate;
class LIBTHEBRANCH_EXPORT CommitActionSnapIn : public SnapIn {
        Q_OBJECT

    public:
        enum class CommitAction {
            CherryPick,
            Revert
        };

        explicit CommitActionSnapIn(RepositoryPtr repo, CommitAction action, QWidget* parent = nullptr);
        ~CommitActionSnapIn();

    private slots:
        void on_titleLabel_backButtonClicked();

        void on_performActionButton_clicked();

    private:
        Ui::CommitActionSnapIn* ui;
        CommitActionSnapInPrivate* d;

        void updateWidgets();
};

#endif // COMMITACTIONSNAPIN_H
