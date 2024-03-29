#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCoroTimer>
#include <QDesktopServices>
#include <QMessageBox>
#include <tapplication.h>
#include <tcommandpalette/tcommandpaletteactionscope.h>
#include <tcommandpalette/tcommandpalettecontroller.h>
#include <tcommandpalette/tcommandpalettedocumentspecificscope.h>
#include <tcsdtools.h>
#include <thelpmenu.h>
#include <tjobmanager.h>
#include <tmessagebox.h>
#include <touchbar/ttouchbar.h>
#include <touchbar/ttouchbaractionitem.h>
#include <tpopover.h>
#include <tsettingswindow/tsettingswindow.h>
#include <twindowtabberbutton.h>

#include <objects/branchuihelper.h>
#include <objects/reference.h>
#include <objects/repository.h>
#include <popovers/snapinpopover.h>
#include <popovers/snapins/branchactionsnapin.h>
#include <popovers/snapins/checkoutsnapin.h>
#include <popovers/snapins/clonerepositorysnapin.h>
#include <popovers/snapins/commitactionsnapin.h>
#include <popovers/snapins/commitsnapin.h>
#include <popovers/snapins/pullsnapin.h>
#include <popovers/snapins/pushsnapin.h>
#include <popovers/snapins/stashsavesnapin.h>
#include <widgets/repositorybrowser.h>

#include <settingspanes/accountspane.h>
#include <settingspanes/commitspane.h>

#include "landingpage.h"
#include "printcontroller.h"

#include <QCoroFuture>

struct MainWindowPrivate {
        tCsdTools csd;

        tCommandPaletteDocumentSpecificScope* branchesScope;
        QMap<RepositoryBrowser*, tWindowTabberButton*> tabButtons;

        tTouchBar* touchBar;
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

    tCommandPaletteActionScope* commandPaletteActionScope;
    auto commandPalette = tCommandPaletteController::defaultController(this, &commandPaletteActionScope);

    d->branchesScope = new tCommandPaletteDocumentSpecificScope(this);
    commandPalette->addScope(d->branchesScope);

    tHelpMenu* helpMenu = new tHelpMenu(this, commandPalette);
    ui->menuBar->addMenu(helpMenu);

#ifdef Q_OS_MAC
    ui->menuButton->setVisible(false);
#else
    ui->menuBar->setVisible(false);
    QMenu* menu = new QMenu(this);

    menu->addAction(ui->actionOpen_Repository);
    menu->addAction(ui->actionClone_Repository);
    menu->addAction(ui->actionClose_Tab);
    menu->addSeparator();
    menu->addAction(ui->actionCommit);
    menu->addAction(ui->actionCheckout);
    menu->addSeparator();
    menu->addAction(ui->actionNew_Branch);
    menu->addAction(ui->actionNew_Tag);
    menu->addSeparator();
    menu->addAction(ui->actionMerge);
    menu->addAction(ui->actionRebase);
    menu->addAction(ui->actionCherry_Pick);
    menu->addSeparator();
    menu->addAction(ui->actionRevert);
    menu->addSeparator();
    menu->addAction(ui->actionOpen_in);
    menu->addSeparator();
    menu->addAction(ui->actionPush);
    menu->addAction(ui->actionPull);
    menu->addSeparator();
    menu->addAction(ui->actionStash);
    menu->addAction(ui->actionDiscard_All_Changes);
    menu->addSeparator();
    menu->addAction(ui->actionPrint);
    menu->addSeparator();
    menu->addMenu(helpMenu);
    menu->addAction(ui->actionSettings);
    menu->addAction(ui->actionExit);

    ui->menuButton->setIcon(tApplication::applicationIcon());
    ui->menuButton->setIconSize(SC_DPI_WT(QSize(24, 24), QSize, this));
    ui->menuButton->setMenu(menu);
#endif

    switch (tApplication::currentPlatform()) {
        case tApplication::Windows:
        case tApplication::WindowsAppPackage:
            ui->actionOpen_in->setText(tr("Open in %1").arg("File Explorer"));
            break;
        case tApplication::MacOS:
            ui->actionOpen_in->setText(tr("Open in %1").arg("Finder"));
            break;
        case tApplication::TheDesk:
            ui->actionOpen_in->setText(tr("Open in %1").arg("theFile"));
            break;
        case tApplication::Flatpak:
        case tApplication::OtherPlatform:
            ui->actionOpen_in->setText(tr("Open in %1").arg("File Manager"));
            break;
    }

    auto landingPage = new LandingPage(this);
    connect(landingPage, &LandingPage::openRepository, this, [this] {
        ui->actionOpen_Repository->trigger();
    });
    connect(landingPage, &LandingPage::cloneRepository, this, [this] {
        ui->actionClone_Repository->trigger();
    });

    ui->stackedWidget->setDefaultWidget(landingPage);
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
    this->setWindowIcon(tApplication::applicationIcon());

    connect(ui->stackedWidget, &tStackedWidget::switchingFrame, this, &MainWindow::updateMenuItems);

    commandPaletteActionScope->addMenuBar(ui->menuBar);

    d->touchBar = new tTouchBar(this);
    d->touchBar->setCustomizationIdentifier(QStringLiteral("com.vicr123.thebranch"));
    d->touchBar->addDefaultItem(new tTouchBarActionItem(QStringLiteral("com.vicr123.thebranch.commit"), ui->actionCommit, tr("Commit"), {}, this));
    d->touchBar->addDefaultItem(new tTouchBarActionItem(QStringLiteral("com.vicr123.thebranch.push"), ui->actionPush, "", {}, this));
    d->touchBar->addDefaultItem(new tTouchBarActionItem(QStringLiteral("com.vicr123.thebranch.pull"), ui->actionPull, "", {}, this));
    d->touchBar->addItem(new tTouchBarActionItem(QStringLiteral("com.vicr123.thebranch.checkout"), ui->actionCheckout, tr("Checkout"), {}, this));
    d->touchBar->addItem(new tTouchBarActionItem(QStringLiteral("com.vicr123.thebranch.merge"), ui->actionMerge, tr("Merge"), {}, this));
    d->touchBar->addItem(new tTouchBarActionItem(QStringLiteral("com.vicr123.thebranch.rebase"), ui->actionRebase, tr("Rebase"), {}, this));
    d->touchBar->addItem(new tTouchBarActionItem(QStringLiteral("com.vicr123.thebranch.cherrypick"), ui->actionCherry_Pick, tr("Cherry Pick"), {}, this));
    d->touchBar->addItem(new tTouchBarActionItem(QStringLiteral("com.vicr123.thebranch.revert"), ui->actionRevert, tr("Revert"), {}, this));
    d->touchBar->addItem(new tTouchBarActionItem(QStringLiteral("com.vicr123.thebranch.branch"), ui->actionNew_Branch, tr("New Branch"), {}, this));
    d->touchBar->addItem(new tTouchBarActionItem(QStringLiteral("com.vicr123.thebranch.tag"), ui->actionNew_Tag, tr("New Tag"), {}, this));

    d->touchBar->attach(this);

    tTouchBar::addCustomizationMenuItem(ui->menuView);

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

    d->branchesScope->registerScope(browser, repo->commandPaletteBranches());

    updateMenuItems();
}

RepositoryBrowser* MainWindow::openNextTab() {
    RepositoryBrowser* browser = qobject_cast<RepositoryBrowser*>(ui->stackedWidget->widget(ui->stackedWidget->currentIndex()));
    if (browser && browser->repository() == nullptr) {
        return browser;
    } else {
        RepositoryBrowser* browser = new RepositoryBrowser(this);
        tWindowTabberButton* initialBrowserTab = new tWindowTabberButton(QIcon(), browser->title(), ui->stackedWidget, browser);

        ui->stackedWidget->addWidget(browser);
        connect(browser, &RepositoryBrowser::titleChanged, this, [=] {
            initialBrowserTab->setText(browser->title());
        });
        connect(browser, &RepositoryBrowser::repositoryChanged, this, &MainWindow::updateMenuItems);

        QAction* commitAction = new QAction(QIcon::fromTheme("commit"), tr("Commit"));
        connect(commitAction, &QAction::triggered, this, [=] {
            SnapInPopover::showSnapInPopover(this, new CommitSnapIn(browser->repository()));
        });
        initialBrowserTab->addAction(commitAction);

        ui->windowTabber->addButton(initialBrowserTab);
        d->tabButtons.insert(browser, initialBrowserTab);

        return browser;
    }
}

void MainWindow::on_actionExit_triggered() {
    tApplication::exit();
}

void MainWindow::on_actionClone_Repository_triggered() {
    CloneRepositorySnapIn* jp = new CloneRepositorySnapIn();
    connect(jp, &CloneRepositorySnapIn::openRepository, this, [=](RepositoryPtr repository) {
        RepositoryBrowser* browser = openNextTab();
        browser->setRepository(repository);
        connect(repository.data(), &Repository::stateChanged, this, &MainWindow::updateMenuItems);

        d->branchesScope->registerScope(browser, repository->commandPaletteBranches());

        updateMenuItems();
    });
    SnapInPopover::showSnapInPopover(this, jp);
}

QCoro::Task<> MainWindow::on_actionOpen_Repository_triggered() {
    try {
        auto repo = co_await Repository::repositoryForDirectoryUi(this);

        RepositoryBrowser* browser = openNextTab();
        browser->setRepository(repo);
        connect(repo.data(), &Repository::stateChanged, this, &MainWindow::updateMenuItems);

        d->branchesScope->registerScope(browser, repo->commandPaletteBranches());

        updateMenuItems();
    } catch (QException ex) {
        // ignore
    }
}

void MainWindow::on_actionCheckout_triggered() {
    SnapInPopover::showSnapInPopover(this, new CheckoutSnapIn(qobject_cast<RepositoryBrowser*>(ui->stackedWidget->currentWidget())->repository()));
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
    ui->actionClose_Tab->setEnabled(enabled);
    ui->actionDiscard_All_Changes->setEnabled(enabled);
    ui->actionOpen_in->setEnabled(enabled);
    ui->actionRevert->setEnabled(enabled);
}

void MainWindow::on_actionCommit_triggered() {
    SnapInPopover::showSnapInPopover(this, new CommitSnapIn(qobject_cast<RepositoryBrowser*>(ui->stackedWidget->currentWidget())->repository()));
}

void MainWindow::on_actionPrint_triggered() {
    PrintController* controller = new PrintController(this);
    controller->confirmAndPerformPrint();
}

void MainWindow::on_actionPush_triggered() {
    SnapInPopover::showPushPopover(this, qobject_cast<RepositoryBrowser*>(ui->stackedWidget->currentWidget())->repository());
}

void MainWindow::on_actionPull_triggered() {
    SnapInPopover::showPullPopover(this, qobject_cast<RepositoryBrowser*>(ui->stackedWidget->currentWidget())->repository());
}

void MainWindow::on_actionStash_triggered() {
    SnapInPopover::showSnapInPopover(this, new StashSaveSnapIn(qobject_cast<RepositoryBrowser*>(ui->stackedWidget->currentWidget())->repository()));
}

void MainWindow::on_actionSettings_triggered() {
    tSettingsWindow window(this);
    window.appendSection(tr("Git"));
    window.appendPane(new CommitsPane());
    window.appendPane(new AccountsPane());
    window.exec();
}

void MainWindow::on_actionDiscard_All_Changes_triggered() {
    BranchUiHelper::discardRepositoryChanges(qobject_cast<RepositoryBrowser*>(ui->stackedWidget->currentWidget())->repository(), this->window());
}

void MainWindow::on_actionNew_Branch_triggered() {
    auto repo = qobject_cast<RepositoryBrowser*>(ui->stackedWidget->currentWidget())->repository();
    if (!repo->head()) {
        tMessageBox* box = new tMessageBox(this->window());
        box->setTitleBarText(tr("HEAD is unborn"));
        box->setMessageText(tr("There is no commit to branch from. Create a commit first, and then branch from it."));
        box->setIcon(QMessageBox::Critical);
        box->exec(true);
        return;
    }
    BranchUiHelper::branch(repo, repo, this);
}

void MainWindow::on_actionClose_Tab_triggered() {
    auto browser = qobject_cast<RepositoryBrowser*>(ui->stackedWidget->currentWidget());
    if (browser) {
        ui->stackedWidget->removeWidget(browser);
        ui->windowTabber->removeButton(d->tabButtons.value(browser));
        d->tabButtons.remove(browser);
        browser->deleteLater();

        updateMenuItems();
    }
}

void MainWindow::on_actionCherry_Pick_triggered() {
    auto browser = qobject_cast<RepositoryBrowser*>(ui->stackedWidget->currentWidget());
    if (browser) {
        SnapInPopover::showSnapInPopover(this, new CommitActionSnapIn(browser->repository(), CommitActionSnapIn::CommitAction::CherryPick));
    }
}

void MainWindow::on_actionRevert_triggered() {
    auto browser = qobject_cast<RepositoryBrowser*>(ui->stackedWidget->currentWidget());
    if (browser) {
        SnapInPopover::showSnapInPopover(this, new CommitActionSnapIn(browser->repository(), CommitActionSnapIn::CommitAction::Revert));
    }
}

void MainWindow::on_actionOpen_in_triggered() {
    auto browser = qobject_cast<RepositoryBrowser*>(ui->stackedWidget->currentWidget());
    if (browser) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(browser->repository()->repositoryPath()));
    }
}

void MainWindow::on_actionMerge_triggered() {
    auto browser = qobject_cast<RepositoryBrowser*>(ui->stackedWidget->currentWidget());
    if (browser) {
        if (!browser->repository()->head()->asBranch()) {
            tMessageBox* box = new tMessageBox(this->window());
            box->setTitleBarText(tr("HEAD is detached"));
            box->setMessageText(tr("There is no branch to merge into. Checkout a branch first, and then merge."));
            box->setIcon(QMessageBox::Critical);
            box->exec(true);
            return;
        }
        SnapInPopover::showSnapInPopover(this, new BranchActionSnapIn(browser->repository(), BranchActionSnapIn::BranchAction::Merge));
    }
}

void MainWindow::on_actionRebase_triggered() {
    auto browser = qobject_cast<RepositoryBrowser*>(ui->stackedWidget->currentWidget());
    if (browser) {
        if (!browser->repository()->head()->asBranch()) {
            tMessageBox* box = new tMessageBox(this->window());
            box->setTitleBarText(tr("HEAD is detached"));
            box->setMessageText(tr("There is no branch to rebase onto. Checkout a branch first, and then rebase."));
            box->setIcon(QMessageBox::Critical);
            box->exec(true);
            return;
        }
        SnapInPopover::showSnapInPopover(this, new BranchActionSnapIn(browser->repository(), BranchActionSnapIn::BranchAction::Rebase));
    }
}
