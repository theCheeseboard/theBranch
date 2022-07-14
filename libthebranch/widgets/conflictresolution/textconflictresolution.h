#ifndef TEXTCONFLICTRESOLUTION_H
#define TEXTCONFLICTRESOLUTION_H

#include "conflictresolutionwidget.h"

struct TextConflictResolutionPrivate;
class TextConflictResolution : public ConflictResolutionWidget {
        Q_OBJECT
    public:
        explicit TextConflictResolution(QString file, QWidget* parent = nullptr);
        ~TextConflictResolution();

    signals:

    private:
        TextConflictResolutionPrivate* d;

        // ConflictResolutionWidget interface
    public:
        bool isConflictResolutionCompleted();
        void applyConflictResolution();
};

#endif // TEXTCONFLICTRESOLUTION_H
