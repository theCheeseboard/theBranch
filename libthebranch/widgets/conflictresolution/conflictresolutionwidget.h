#ifndef CONFLICTRESOLUTIONWIDGET_H
#define CONFLICTRESOLUTIONWIDGET_H

#include <QWidget>

class ConflictResolutionWidget : public QWidget {
        Q_OBJECT
    public:
        explicit ConflictResolutionWidget(QWidget* parent = nullptr);

        virtual bool isConflictResolutionCompleted() = 0;
        virtual void applyConflictResolution() = 0;

    signals:
        void conflictResolutionCompletedChanged();
};

#endif // CONFLICTRESOLUTIONWIDGET_H
