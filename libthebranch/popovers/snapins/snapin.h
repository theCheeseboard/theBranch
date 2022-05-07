#ifndef SNAPIN_H
#define SNAPIN_H

#include <QWidget>

class SnapIn : public QWidget {
        Q_OBJECT
    public:
        explicit SnapIn(QWidget* parent = nullptr);

    signals:
        void done();
};

#endif // SNAPIN_H
