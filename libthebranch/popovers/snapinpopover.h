#ifndef SNAPINPOPOVER_H
#define SNAPINPOPOVER_H

#include <QWidget>

namespace Ui {
    class SnapInPopover;
}

class SnapIn;
struct SnapInPopoverPrivate;
class SnapInPopover : public QWidget {
        Q_OBJECT

    public:
        explicit SnapInPopover(QWidget* parent = nullptr);
        ~SnapInPopover();

        void pushSnapIn(SnapIn* snapin);

    signals:
        void done();

    private:
        Ui::SnapInPopover* ui;
        SnapInPopoverPrivate* d;
};

#endif // SNAPINPOPOVER_H
