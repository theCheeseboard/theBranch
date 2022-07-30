#include "textconflictresolution.h"

#include <merge/textmergetool.h>

#include <QBoxLayout>
#include <QFile>

struct TextConflictResolutionPrivate {
        TextMergeTool* mergeTool;
        QString file;
};

TextConflictResolution::TextConflictResolution(QString file, QWidget* parent) :
    ConflictResolutionWidget{parent} {
    d = new TextConflictResolutionPrivate();
    d->file = file;
    d->mergeTool = new TextMergeTool();

    QFile initialFile(d->file);
    initialFile.open(QFile::ReadOnly);

    QString leftContent;
    QString rightContent;

    auto lines = QString(initialFile.readAll()).split("\n");
    bool writeToLeftSide = true;
    bool writeToRightSide = true;
    for (const auto& line : lines) {
        if (line.startsWith("<<<<<<< ")) {
            writeToLeftSide = true;
            writeToRightSide = false;
        } else if (line.startsWith("=======")) {
            writeToLeftSide = false;
            writeToRightSide = true;
        } else if (line.startsWith(">>>>>>> ")) {
            writeToLeftSide = true;
            writeToRightSide = true;
        } else {
            if (writeToLeftSide) leftContent.append(line + "\n");
            if (writeToRightSide) rightContent.append(line + "\n");
        }
    }

    //    d->mergeTool->setLeftSide(leftContent);
    //    d->mergeTool->setRightSide(rightContent);

    d->mergeTool->loadDiff(leftContent, rightContent);

    initialFile.close();

    auto* layout = new QBoxLayout(QBoxLayout::TopToBottom);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(d->mergeTool);
    this->setLayout(layout);

    connect(d->mergeTool, &TextMergeTool::conflictResolutionCompletedChanged, this, &TextConflictResolution::conflictResolutionCompletedChanged);
}

TextConflictResolution::~TextConflictResolution() {
    delete d;
}

bool TextConflictResolution::isConflictResolutionCompleted() {
    return d->mergeTool->isConflictResolutionCompleted();
}

void TextConflictResolution::applyConflictResolution() {
    QFile outputFile(d->file);
    outputFile.open(QFile::WriteOnly);
    outputFile.write(d->mergeTool->conflictResolution().toUtf8());
    outputFile.close();
}
