#include "snapin.h"

struct SnapInPrivate {
        SnapInPopover* popover;
};

SnapIn::SnapIn(QWidget* parent) :
    QWidget{parent} {
    d = new SnapInPrivate();
}

SnapIn::~SnapIn() {
    delete d;
}

void SnapIn::snapinShown() {
}

void SnapIn::setParentPopover(SnapInPopover* popover) {
    d->popover = popover;
}

SnapInPopover* SnapIn::parentPopover() {
    return d->popover;
}
