#include "snapinpopover.h"
#include "ui_snapinpopover.h"

#include "snapins/snapin.h"
#include <tpopover.h>

struct SnapInPopoverPrivate {
};

SnapInPopover::SnapInPopover(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::SnapInPopover) {
    ui->setupUi(this);
    d = new SnapInPopoverPrivate();

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
}

SnapInPopover::~SnapInPopover() {
    delete d;
    delete ui;
}

void SnapInPopover::pushSnapIn(SnapIn* snapin) {
    ui->stackedWidget->addWidget(snapin);
    ui->stackedWidget->setCurrentWidget(snapin);

    snapin->snapinShown();

    connect(snapin, &SnapIn::done, this, [this] {
        emit done();
    });
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
