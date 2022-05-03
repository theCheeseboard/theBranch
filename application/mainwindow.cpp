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
    menu->addSeperator();
    menu->addMenu(helpMenu);
    menu->addAction(ui->actionExit);

    ui->menuButton->setIcon(tApplication::applicationIcon());
    ui->menuButton->setIconSize(SC_DPI_T(QSize(24, 24), QSize));
    ui->menuButton->setMenu(menu);
#endif

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
    this->setWindowIcon(tApplication::applicationIcon());

    RepositoryBrowser* initialBrowser = new RepositoryBrowser(this);
    ui->stackedWidget->addWidget(initialBrowser);
    tWindowTabberButton* initialBrowserTab = new tWindowTabberButton(QIcon(), initialBrowser->title(), ui->stackedWidget, initialBrowser);
    ui->windowTabber->addButton(initialBrowserTab);
    connect(initialBrowser, &RepositoryBrowser::titleChanged, this, [=] {
        initialBrowserTab->setText(initialBrowser->title());
    });
}

MainWindow::~MainWindow() {
    delete ui;
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
        RepositoryBrowser* browser = qobject_cast<RepositoryBrowser*>(ui->stackedWidget->widget(ui->stackedWidget->currentIndex()));
        if (browser->repository() == nullptr) {
            browser->setRepository(repository);
        } else {
            RepositoryBrowser* browser = new RepositoryBrowser(this);
            browser->setRepository(repository);
            ui->stackedWidget->addWidget(browser);
            tWindowTabberButton* initialBrowserTab = new tWindowTabberButton(QIcon(), browser->title(), ui->stackedWidget, browser);
            ui->windowTabber->addButton(initialBrowserTab);
            connect(browser, &RepositoryBrowser::titleChanged, this, [=] {
                initialBrowserTab->setText(browser->title());
            });
        }
    });
    connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
    connect(popover, &tPopover::dismissed, jp, &CloneRepositoryPopover::deleteLater);
    popover->show(this->window());
}

void MainWindow::on_actionOpen_Repository_triggered() {
    Repository::repositoryForDirectoryUi(this)->then([=](RepositoryPtr repo) {
        RepositoryBrowser* browser = qobject_cast<RepositoryBrowser*>(ui->stackedWidget->widget(ui->stackedWidget->currentIndex()));
        if (browser->repository() == nullptr) {
            browser->setRepository(repo);
        } else {
            RepositoryBrowser* browser = new RepositoryBrowser(this);
            browser->setRepository(repo);
            ui->stackedWidget->addWidget(browser);
            tWindowTabberButton* initialBrowserTab = new tWindowTabberButton(QIcon(), browser->title(), ui->stackedWidget, browser);
            ui->windowTabber->addButton(initialBrowserTab);
            connect(browser, &RepositoryBrowser::titleChanged, this, [=] {
                initialBrowserTab->setText(browser->title());
            });
        }
    });
}
