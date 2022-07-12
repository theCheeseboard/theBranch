#ifndef SNAPINPOPOVER_H
#define SNAPINPOPOVER_H

#include <QWidget>
#include "libthebranch_global.h"

namespace Ui {
    class SnapInPopover;
}

class SnapIn;
struct SnapInPopoverPrivate;
class LIBTHEBRANCH_EXPORT SnapInPopover : public QWidget {
        Q_OBJECT

    public:
        explicit SnapInPopover(QWidget* parent = nullptr);
        ~SnapInPopover();

        void pushSnapIn(SnapIn* snapin);

        static void showSnapInPopover(QWidget* parent, SnapIn* snapin);

    signals:
        void done();

    private:
        Ui::SnapInPopover* ui;
        SnapInPopoverPrivate* d;
};

#endif // SNAPINPOPOVER_H
