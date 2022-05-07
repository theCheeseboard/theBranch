#include "snapinpopover.h"
#include "ui_snapinpopover.h"

#include "snapins/snapin.h"

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

    connect(snapin, &SnapIn::done, this, [=] {
        emit done();
    });
}
