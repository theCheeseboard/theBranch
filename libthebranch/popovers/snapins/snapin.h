#ifndef SNAPIN_H
#define SNAPIN_H

#include "libthebranch_global.h"
#include <QWidget>
#include <touchbar/ttouchbar.h>

class SnapInPopover;
struct SnapInPrivate;
class LIBTHEBRANCH_EXPORT SnapIn : public QWidget {
        Q_OBJECT
    public:
        explicit SnapIn(QWidget* parent = nullptr);
        ~SnapIn();

        virtual void snapinShown();

        void setParentPopover(SnapInPopover* popover);
        SnapInPopover* parentPopover();

        virtual tTouchBar* touchBar() {
            return nullptr;
        }

    signals:
        void done();

    private:
        SnapInPrivate* d;
};

#endif // SNAPIN_H
