#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCoroTask>
#include <QFuture>
#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class RepositoryBrowser;
struct MainWindowPrivate;
class MainWindow : public QMainWindow {
        Q_OBJECT

    public:
        explicit MainWindow(QWidget* parent = nullptr);
        ~MainWindow();

        void openRepo(QString path);

        RepositoryBrowser* openNextTab();

    private slots:
        void on_actionExit_triggered();

        void on_actionClone_Repository_triggered();

        QCoro::Task<> on_actionOpen_Repository_triggered();

        void on_actionCheckout_triggered();

        void on_actionCommit_triggered();

        void on_actionPrint_triggered();

        void on_actionPush_triggered();

        void on_actionPull_triggered();

        void on_actionStash_triggered();

        void on_actionSettings_triggered();

        void on_actionDiscard_All_Changes_triggered();

        void on_actionNew_Branch_triggered();

        void on_actionClose_Tab_triggered();

        void on_actionCherry_Pick_triggered();

        void on_actionRevert_triggered();

        void on_actionOpen_in_triggered();

        void on_actionMerge_triggered();

        void on_actionRebase_triggered();

    private:
        Ui::MainWindow* ui;
        MainWindowPrivate* d;

        void updateMenuItems();
};

#endif // MAINWINDOW_H
