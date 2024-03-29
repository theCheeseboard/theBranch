#ifndef SNAPINPOPOVER_H
#define SNAPINPOPOVER_H

#include "libthebranch_global.h"
#include "objects/forward_declares.h"
#include <QWidget>

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

        InformationRequiredCallback getInformationRequiredCallback();

        static void showSnapInPopover(QWidget* parent, SnapIn* snapin);
        static void showPushPopover(QWidget* parent, RepositoryPtr repo);
        static void showPullPopover(QWidget* parent, RepositoryPtr repo);

    signals:
        void done();

    private:
        Ui::SnapInPopover* ui;
        SnapInPopoverPrivate* d;

        void currentChanged(int switchTo);
};

#endif // SNAPINPOPOVER_H
