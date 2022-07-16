#include "snapinpopover.h"
#include "ui_snapinpopover.h"

#include "snapins/snapin.h"
#include <QTimer>
#include <tpopover.h>

struct SnapInPopoverPrivate {
        int numSnapins = 0;
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
                    widget->snapinShown();
                    ui->stackedWidget->setCurrentWidget(widget);
                }

                QTimer::singleShot(500, this, [this, snapin] {
                    ui->stackedWidget->removeWidget(snapin);
                });
            }
        },
        Qt::QueuedConnection);
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
