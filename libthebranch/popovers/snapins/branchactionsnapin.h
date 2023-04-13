#ifndef BRANCHACTIONSNAPIN_H
#define BRANCHACTIONSNAPIN_H

#include "libthebranch_global.h"
#include "objects/forward_declares.h"
#include "snapin.h"

namespace Ui {
    class BranchActionSnapIn;
}

struct BranchActionSnapInPrivate;
class LIBTHEBRANCH_EXPORT BranchActionSnapIn : public SnapIn {
        Q_OBJECT

    public:
        enum class BranchAction {
            Merge,
            Rebase
        };

        explicit BranchActionSnapIn(RepositoryPtr repo, BranchAction action, QWidget* parent = nullptr);
        ~BranchActionSnapIn();

    private slots:
        void on_performActionButton_clicked();

        void on_titleLabel_backButtonClicked();

    private:
        Ui::BranchActionSnapIn* ui;
        BranchActionSnapInPrivate* d;
};

#endif // BRANCHACTIONSNAPIN_H
