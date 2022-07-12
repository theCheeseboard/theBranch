#ifndef COMMITSNAPIN_H
#define COMMITSNAPIN_H

#include "objects/forward_declares.h"
#include "snapin.h"
#include "libthebranch_global.h"

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

    private:
        Ui::CommitSnapIn* ui;
        CommitSnapInPrivate* d;

        void performCommit();
};

#endif // COMMITSNAPIN_H
