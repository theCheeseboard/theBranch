#ifndef TEXTCONFLICTRESOLUTION_H
#define TEXTCONFLICTRESOLUTION_H

#include "conflictresolutionwidget.h"

namespace Ui {
    class TextConflictResolution;
}

struct TextConflictResolutionPrivate;
class TextConflictResolution : public ConflictResolutionWidget {
        Q_OBJECT

    public:
        explicit TextConflictResolution(QString file, QWidget* parent = nullptr);
        ~TextConflictResolution();

    private:
        Ui::TextConflictResolution* ui;
        TextConflictResolutionPrivate* d;

        void loadFile();
        void loadResolutionZones();

        QList<QPolygon> renderResolutionZones();

        // ConflictResolutionWidget interface
    public:
        bool isConflictResolutionCompleted();

        // QObject interface
    public:
        bool eventFilter(QObject* watched, QEvent* event);
};

#endif // TEXTCONFLICTRESOLUTION_H
