#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <tapplication.h>
#include <tcsdtools.h>
#include <thelpmenu.h>
#include <tjobmanager.h>
#include <tpopover.h>
#include <twindowtabberbutton.h>

#include <objects/repository.h>
#include <popovers/clonerepositorypopover.h>
#include <popovers/snapinpopover.h>
#include <popovers/snapins/checkoutsnapin.h>
#include <widgets/repositorybrowser.h>

struct MainWindowPrivate {
        tCsdTools csd;
};

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);

    d = new MainWindowPrivate();
    d->csd.installMoveAction(ui->topWidget);
    d->csd.installResizeAction(this);

    if (tCsdGlobal::windowControlsEdge() == tCsdGlobal::Left) {
        ui->leftCsdLayout->addWidget(d->csd.csdBoxForWidget(this));
    } else {
        ui->rightCsdLayout->addWidget(d->csd.csdBoxForWidget(this));
    }

    ui->jobButtonLayout->addWidget(tJobManager::makeJobButton());

    this->resize(SC_DPI_WT(this->size(), QSize, this));

    //    ui->windowTabber->addButton(new tWindowTabberButton(QIcon::fromTheme("view-media-track"), tr("Tracks"), ui->stackedWidget, ui->tracksPage));
    //    ui->windowTabber->addButton(new tWindowTabberButton(QIcon::fromTheme("view-media-artist"), tr("Artists"), ui->stackedWidget, ui->artistsPage));
    //    ui->windowTabber->addButton(new tWindowTabberButton(QIcon::fromTheme("media-album-cover"), tr("Albums"), ui->stackedWidget, ui->albumsPage));
    //    ui->windowTabber->addButton(new tWindowTabberButton(QIcon::fromTheme("view-media-playlist"), tr("Playlists"), ui->stackedWidget, ui->playlistsPage));
    //    ui->windowTabber->addButton(new tWindowTabberButton(QIcon::fromTheme("view-list-details"), tr("Other Sources"), ui->stackedWidget, ui->otherSourcesPage));

#ifdef Q_OS_MAC
    ui->menuBar->addMenu(new tHelpMenu(this));
    ui->menuButton->setVisible(false);
#else
    ui->menuBar->setVisible(false);
    QMenu* menu = new QMenu(this);

    tHelpMenu* helpMenu = new tHelpMenu(this);

    menu->addAction(ui->actionOpen_Repository);
    menu->addAction(ui->actionClone_Repository);
    menu->addSeparator();
    menu->addMenu(helpMenu);
    menu->addAction(ui->actionExit);

    ui->menuButton->setIcon(tApplication::applicationIcon());
    ui->menuButton->setIconSize(SC_DPI_T(QSize(24, 24), QSize));
    ui->menuButton->setMenu(menu);
#endif

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
    this->setWindowIcon(tApplication::applicationIcon());

    connect(ui->stackedWidget, &tStackedWidget::switchingFrame, this, &MainWindow::updateMenuItems);

    openNextTab();
    updateMenuItems();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::openRepo(QString path) {
    RepositoryPtr repo = Repository::repositoryForDirectory(path);
    connect(repo.data(), &Repository::stateChanged, this, &MainWindow::updateMenuItems);

    RepositoryBrowser* browser = openNextTab();
    browser->setRepository(repo);

    updateMenuItems();
}

RepositoryBrowser* MainWindow::openNextTab() {
    RepositoryBrowser* browser = qobject_cast<RepositoryBrowser*>(ui->stackedWidget->widget(ui->stackedWidget->currentIndex()));
    if (browser && browser->repository() == nullptr) {
        return browser;
    } else {
        RepositoryBrowser* browser = new RepositoryBrowser(this);
        ui->stackedWidget->addWidget(browser);
        tWindowTabberButton* initialBrowserTab = new tWindowTabberButton(QIcon(), browser->title(), ui->stackedWidget, browser);
        ui->windowTabber->addButton(initialBrowserTab);
        connect(browser, &RepositoryBrowser::titleChanged, this, [=] {
            initialBrowserTab->setText(browser->title());
        });
        connect(browser, &RepositoryBrowser::repositoryChanged, this, &MainWindow::updateMenuItems);

        return browser;
    }
}

void MainWindow::on_actionExit_triggered() {
    tApplication::exit();
}

void MainWindow::on_actionClone_Repository_triggered() {
    CloneRepositoryPopover* jp = new CloneRepositoryPopover();
    tPopover* popover = new tPopover(jp);
    popover->setPopoverWidth(SC_DPI_W(-200, this));
    popover->setPopoverSide(tPopover::Bottom);
    connect(jp, &CloneRepositoryPopover::done, popover, &tPopover::dismiss);
    connect(jp, &CloneRepositoryPopover::openRepository, this, [=](RepositoryPtr repository) {
        RepositoryBrowser* browser = openNextTab();
        browser->setRepository(repository);
        connect(repository.data(), &Repository::stateChanged, this, &MainWindow::updateMenuItems);

        updateMenuItems();
    });
    connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
    connect(popover, &tPopover::dismissed, jp, &CloneRepositoryPopover::deleteLater);
    popover->show(this->window());
}

void MainWindow::on_actionOpen_Repository_triggered() {
    Repository::repositoryForDirectoryUi(this)->then([=](RepositoryPtr repo) {
        RepositoryBrowser* browser = openNextTab();
        browser->setRepository(repo);
        connect(repo.data(), &Repository::stateChanged, this, &MainWindow::updateMenuItems);

        updateMenuItems();
    });
}

void MainWindow::on_actionCheckout_triggered() {
    SnapInPopover* jp = new SnapInPopover();
    jp->pushSnapIn(new CheckoutSnapIn(qobject_cast<RepositoryBrowser*>(ui->stackedWidget->currentWidget())->repository()));

    tPopover* popover = new tPopover(jp);
    popover->setPopoverWidth(SC_DPI_W(-200, this));
    popover->setPopoverSide(tPopover::Bottom);
    connect(jp, &SnapInPopover::done, popover, &tPopover::dismiss);
    connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
    connect(popover, &tPopover::dismissed, jp, &SnapInPopover::deleteLater);
    popover->show(this->window());
}

void MainWindow::updateMenuItems() {
    bool enabled = true;
    RepositoryBrowser* repoBrowser = qobject_cast<RepositoryBrowser*>(ui->stackedWidget->currentWidget());
    if (repoBrowser) {
        RepositoryPtr repo = repoBrowser->repository();
        if (repo) {
            if (repo->state() == Repository::Cloning || repo->state() == Repository::Invalid) {
                enabled = false;
            }
        } else {
            enabled = false;
        }
    } else {
        enabled = false;
    }

    ui->actionNew_Branch->setEnabled(enabled);
    ui->actionNew_Tag->setEnabled(enabled);
    ui->actionMerge->setEnabled(enabled);
    ui->actionRebase->setEnabled(enabled);
    ui->actionCherry_Pick->setEnabled(enabled);
    ui->actionCommit->setEnabled(enabled);
    ui->actionCheckout->setEnabled(enabled);
    ui->actionStash->setEnabled(enabled);
    ui->actionPush->setEnabled(enabled);
    ui->actionPull->setEnabled(enabled);
}
