#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFuture>
#include <QMainWindow>
#include <QCoroTask>

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

    private:
        Ui::MainWindow* ui;
        MainWindowPrivate* d;

        void updateMenuItems();
};

#endif // MAINWINDOW_H
