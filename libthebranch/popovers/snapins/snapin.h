#ifndef SNAPIN_H
#define SNAPIN_H

#include <QWidget>

class SnapIn : public QWidget {
        Q_OBJECT
    public:
        explicit SnapIn(QWidget* parent = nullptr);

        virtual void snapinShown();

    signals:
        void done();
};

#endif // SNAPIN_H
