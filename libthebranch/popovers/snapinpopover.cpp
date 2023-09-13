#include "snapinpopover.h"
#include "ui_snapinpopover.h"

#include <QCoroSignal>
#include <QException>
#include <QTimer>
#include <tmessagebox.h>
#include <tpopover.h>

#include "objects/reference.h"
#include "objects/repository.h"

#include "snapins/pullsnapin.h"
#include "snapins/pushsnapin.h"
#include "snapins/snapin.h"
#include "snapins/sshcertchecksnapin.h"
#include "snapins/sshkeyselectionsnapin.h"
#include "snapins/usernamepasswordsnapin.h"

#include <touchbar/ttouchbar.h>

struct SnapInPopoverPrivate {
        int numSnapins = 0;
        SnapIn* currentSnapIn;

        tTouchBar* currentTouchBar = nullptr;
};

SnapInPopover::SnapInPopover(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::SnapInPopover) {
    ui->setupUi(this);
    d = new SnapInPopoverPrivate();

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
    connect(ui->stackedWidget, &tStackedWidget::switchingFrame, this, &SnapInPopover::currentChanged);
    connect(ui->stackedWidget, &tStackedWidget::currentChanged, this, &SnapInPopover::currentChanged);
}

SnapInPopover::~SnapInPopover() {
    if (d->currentTouchBar) {
        d->currentTouchBar->detach();
    }
    delete d;
    delete ui;
}

void SnapInPopover::pushSnapIn(SnapIn* snapin) {
    d->numSnapins++;
    snapin->setParentPopover(this);
    ui->stackedWidget->addWidget(snapin);
    ui->stackedWidget->setCurrentWidget(snapin);

    snapin->snapinShown();

    connect(
        snapin, &SnapIn::done, this, [this, snapin] {
            d->numSnapins--;
            if (d->numSnapins == 0) {
                emit done();
            } else {
                if (ui->stackedWidget->currentIndex() == ui->stackedWidget->indexOf(snapin)) {
                    auto newIndex = ui->stackedWidget->indexOf(snapin) + 1;
                    if (newIndex == ui->stackedWidget->count()) newIndex = ui->stackedWidget->count() - 2;

                    auto widget = static_cast<SnapIn*>(ui->stackedWidget->widget(newIndex));
                    ui->stackedWidget->setCurrentWidget(widget);
                }

                QTimer::singleShot(500, this, [this, snapin] {
                    ui->stackedWidget->removeWidget(snapin);
                });
            }
        },
        Qt::QueuedConnection);
}

InformationRequiredCallback SnapInPopover::getInformationRequiredCallback() {
    return [this](QVariantMap params) -> QCoro::Task<QVariantMap> {
        InformationRequestSnapIn* snapin;

        auto type = params.value("type").toString();
        if (type == "credential") {
            auto credType = params.value("credType").toString();
            if (credType == "ssh-key") {
                snapin = new SshKeySelectionSnapIn(params);
            } else if (credType == "username-password") {
                snapin = new UsernamePasswordSnapIn(params);
            }
        } else if (type == "certcheck") {
            snapin = new SshCertCheckSnapIn(params);
        }

        this->pushSnapIn(snapin);
        auto response = co_await qCoro(snapin, &InformationRequestSnapIn::response);
        if (response.isEmpty()) throw QException();

        co_return response;
    };
}

void SnapInPopover::showSnapInPopover(QWidget* parent, SnapIn* snapin) {
    SnapInPopover* jp = new SnapInPopover();

    tPopover* popover = new tPopover(jp);
    popover->setPopoverWidth(SC_DPI_W(-200, parent));
    popover->setPopoverSide(tPopover::Bottom);
    connect(jp, &SnapInPopover::done, popover, &tPopover::dismiss);
    connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
    connect(popover, &tPopover::dismissed, jp, &SnapInPopover::deleteLater);
    popover->show(parent->window());

    jp->pushSnapIn(snapin);
}

void SnapInPopover::showPushPopover(QWidget* parent, RepositoryPtr repo) {
    // Guard the push popover with a check to ensure that we are not on a detached HEAD
    if (!repo->head() || !repo->head()->asBranch()) {
        tMessageBox* box = new tMessageBox(parent->window());
        box->setTitleBarText(tr("HEAD is detached"));
        box->setMessageText(tr("There is no branch to push. Checkout a branch first, and then push the repository."));
        box->setIcon(QMessageBox::Critical);
        box->exec(true);
        return;
    }

    showSnapInPopover(parent, new PushSnapIn(repo));
}

void SnapInPopover::showPullPopover(QWidget* parent, RepositoryPtr repo) {
    // Guard the push popover with a check to ensure that we are not on a detached HEAD
    if (!repo->head() || !repo->head()->asBranch()) {
        tMessageBox* box = new tMessageBox(parent->window());
        box->setTitleBarText(tr("HEAD is detached"));
        box->setMessageText(tr("There is no branch to pull. Checkout a branch first, and then pull the repository."));
        box->setIcon(QMessageBox::Critical);
        box->exec(true);
        return;
    }

    showSnapInPopover(parent, new PullSnapIn(repo));
}

void SnapInPopover::currentChanged(int switchTo) {
    auto widget = static_cast<SnapIn*>(ui->stackedWidget->widget(switchTo));
    if (widget != d->currentSnapIn) widget->snapinShown();
    d->currentSnapIn = widget;

    if (d->currentTouchBar) {
        d->currentTouchBar->detach();
    }
    d->currentTouchBar = widget->touchBar();
    if (d->currentTouchBar) {
        d->currentTouchBar->attach(this);
    }
}
