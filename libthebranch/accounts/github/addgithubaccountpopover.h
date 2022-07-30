#ifndef ADDGITHUBACCOUNTPOPOVER_H
#define ADDGITHUBACCOUNTPOPOVER_H

#include <QCoroTask>
#include <QWidget>

namespace Ui {
    class AddGithubAccountPopover;
}

class AddGithubAccountPopover : public QWidget {
        Q_OBJECT

    public:
        explicit AddGithubAccountPopover(QWidget* parent = nullptr);
        ~AddGithubAccountPopover();

    signals:
        void done();

    private slots:
        void on_titleLabel_backButtonClicked();

        QCoro::Task<> on_addButton_clicked();

        void on_createPatButton_clicked();

    private:
        Ui::AddGithubAccountPopover* ui;
};

#endif // ADDGITHUBACCOUNTPOPOVER_H
