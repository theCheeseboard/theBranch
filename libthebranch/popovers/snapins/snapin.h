#ifndef SNAPIN_H
#define SNAPIN_H

#include <QWidget>
#include "libthebranch_global.h"

class LIBTHEBRANCH_EXPORT SnapIn : public QWidget {
        Q_OBJECT
    public:
        explicit SnapIn(QWidget* parent = nullptr);

        virtual void snapinShown();

    signals:
        void done();
};

#endif // SNAPIN_H
