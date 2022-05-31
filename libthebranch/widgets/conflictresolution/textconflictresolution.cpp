#include "textconflictresolution.h"
#include "ui_textconflictresolution.h"

#include "conflictresolutiontexteditorrenderstep.h"
#include <QActionGroup>
#include <QFile>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QScrollBar>
#include <libcontemporary_global.h>

struct TextConflictResolutionPrivate {
        struct ConflictResolutionZone {
                enum ResolveDirection {
                    NoResolution,
                    ResolveLeft,
                    ResolveRight,
                    ResolveLeftThenRight,
                    ResolveRightThenLeft
                };

                QString leftContent;
                QString rightContent;
                ResolveDirection resolveDirection = NoResolution;

                QString effectiveLeftContent() const;
        };

        QString file;
        QList<ConflictResolutionZone> resolutionZones;
};

TextConflictResolution::TextConflictResolution(QString file, QWidget* parent) :
    ConflictResolutionWidget(parent),
    ui(new Ui::TextConflictResolution) {
    ui->setupUi(this);
    d = new TextConflictResolutionPrivate();
    d->file = file;

    ui->resolutionArea->setFixedWidth(SC_DPI_W(50, this));

    ui->leftTextEditor->pushRenderStep(new ConflictResolutionTextEditorRenderStep(ui->leftTextEditor));
    ui->rightTextEditor->pushRenderStep(new ConflictResolutionTextEditorRenderStep(ui->rightTextEditor));

    connect(ui->leftTextEditor->verticalScrollBar(), &QScrollBar::valueChanged, this, [=] {
        ui->resolutionArea->update();
        ui->rightTextEditor->verticalScrollBar()->setValue(ui->leftTextEditor->verticalScrollBar()->value());
    });
    connect(ui->rightTextEditor->verticalScrollBar(), &QScrollBar::valueChanged, this, [=] {
        ui->resolutionArea->update();

        // TODO: Something smarter with the scrolling
        ui->leftTextEditor->verticalScrollBar()->setValue(ui->rightTextEditor->verticalScrollBar()->value());
    });

    ui->resolutionArea->installEventFilter(this);

    loadFile();
}

TextConflictResolution::~TextConflictResolution() {
    delete ui;
    delete d;
}

void TextConflictResolution::loadFile() {
    QFile initialFile(d->file);
    initialFile.open(QFile::ReadOnly);
    auto lines = QString(initialFile.readAll()).split("\n");
    initialFile.close();

    bool writeToLeftSide = true;
    TextConflictResolutionPrivate::ConflictResolutionZone currentZone;
    for (const auto& line : lines) {
        if (line.startsWith("<<<<<<< ")) {
            if (!currentZone.leftContent.isEmpty()) {
                currentZone.rightContent = currentZone.leftContent;
                d->resolutionZones.append(currentZone);
            }
            currentZone = TextConflictResolutionPrivate::ConflictResolutionZone();
        } else if (line.startsWith("=======")) {
            writeToLeftSide = false;
        } else if (line.startsWith(">>>>>>> ")) {
            writeToLeftSide = true;

            d->resolutionZones.append(currentZone);
            currentZone = TextConflictResolutionPrivate::ConflictResolutionZone();
        } else {
            if (writeToLeftSide) {
                currentZone.leftContent.append(line + "\n");
            } else {
                currentZone.rightContent.append(line + "\n");
            }
        }
    }
    if (!currentZone.leftContent.isEmpty()) {
        currentZone.rightContent = currentZone.leftContent;
        d->resolutionZones.append(currentZone);
    }

    this->loadResolutionZones();
}

void TextConflictResolution::loadResolutionZones() {
    int leftScroll = ui->leftTextEditor->verticalScrollBar()->value();
    int rightScroll = ui->rightTextEditor->verticalScrollBar()->value();
    ui->leftTextEditor->clearLineProperties("conflictResolution");
    ui->rightTextEditor->clearLineProperties("conflictResolution");

    QString leftContent;
    QString rightContent;
    for (const auto& zone : d->resolutionZones) {
        leftContent.append(zone.effectiveLeftContent());
        rightContent.append(zone.rightContent);
    }
    ui->leftTextEditor->setText(leftContent);
    ui->rightTextEditor->setText(rightContent);

    int leftLine = 0;
    int rightLine = 0;
    for (const auto& zone : d->resolutionZones) {
        int leftLength = zone.effectiveLeftContent().split("\n").length() - 1;
        int rightLength = zone.rightContent.split("\n").length() - 1;

        // Check if this is a conflict resolution zone
        if (zone.leftContent != zone.rightContent) {
            int base = zone.resolveDirection == TextConflictResolutionPrivate::ConflictResolutionZone::NoResolution ? 0 : 5;

            if (leftLength == 0) {
                ui->leftTextEditor->setLineProperty(leftLine, "conflictResolution", 4 + base);
            } else {
                for (auto i = 0; i < leftLength; i++) {
                    if (i == 0 && i == leftLength - 1) {
                        ui->leftTextEditor->setLineProperty(leftLine + i, "conflictResolution", 3 + base);
                    } else if (i == 0) {
                        ui->leftTextEditor->setLineProperty(leftLine + i, "conflictResolution", 0 + base);
                    } else if (i == leftLength - 1) {
                        ui->leftTextEditor->setLineProperty(leftLine + i, "conflictResolution", 1 + base);
                    } else {
                        ui->leftTextEditor->setLineProperty(leftLine + i, "conflictResolution", 2 + base);
                    }
                }
            }

            if (rightLength == 0) {
                ui->rightTextEditor->setLineProperty(rightLine, "conflictResolution", 4 + base);
            } else {
                for (auto i = 0; i < rightLength; i++) {
                    if (i == 0 && i == rightLength - 1) {
                        ui->rightTextEditor->setLineProperty(rightLine + i, "conflictResolution", 3 + base);
                    } else if (i == 0) {
                        ui->rightTextEditor->setLineProperty(rightLine + i, "conflictResolution", 0 + base);
                    } else if (i == rightLength - 1) {
                        ui->rightTextEditor->setLineProperty(rightLine + i, "conflictResolution", 1 + base);
                    } else {
                        ui->rightTextEditor->setLineProperty(rightLine + i, "conflictResolution", 2 + base);
                    }
                }
            }
        }

        leftLine += leftLength;
        rightLine += rightLength;
    }

    ui->leftTextEditor->verticalScrollBar()->setValue(leftScroll);
    ui->rightTextEditor->verticalScrollBar()->setValue(rightScroll);
    ui->resolutionArea->update();
}

QList<QPolygon> TextConflictResolution::renderResolutionZones() {
    QList<QPolygon> polygons;

    int leftLine = 0;
    int rightLine = 0;
    for (const auto& zone : d->resolutionZones) {
        int leftLength = zone.effectiveLeftContent().split("\n").length() - 1;
        int rightLength = zone.rightContent.split("\n").length() - 1;

        // Check if this is a conflict resolution zone
        if (zone.leftContent == zone.rightContent) {
            polygons.append(QPolygon());
        } else {
            QPolygon poly;

            poly.append({QPoint(0, ui->leftTextEditor->lineTop(leftLine) - ui->leftTextEditor->verticalScrollBar()->value()),
                QPoint(ui->resolutionArea->width(), ui->rightTextEditor->lineTop(rightLine) - ui->rightTextEditor->verticalScrollBar()->value()),
                QPoint(ui->resolutionArea->width(), ui->rightTextEditor->lineTop(rightLine + rightLength) - ui->rightTextEditor->verticalScrollBar()->value()),
                QPoint(0, ui->leftTextEditor->lineTop(leftLine + leftLength) - ui->leftTextEditor->verticalScrollBar()->value())});

            polygons.append(poly);
        }

        leftLine += leftLength;
        rightLine += rightLength;
    }
    return polygons;
}

bool TextConflictResolution::isConflictResolutionCompleted() {
    return false;
}

QString TextConflictResolutionPrivate::ConflictResolutionZone::effectiveLeftContent() const {
    switch (resolveDirection) {
        case TextConflictResolutionPrivate::ConflictResolutionZone::NoResolution:
            return leftContent;
            break;
        case TextConflictResolutionPrivate::ConflictResolutionZone::ResolveLeft:
            return leftContent;
            break;
        case TextConflictResolutionPrivate::ConflictResolutionZone::ResolveRight:
            return rightContent;
            break;
        case TextConflictResolutionPrivate::ConflictResolutionZone::ResolveLeftThenRight:
            return leftContent + "\n" + rightContent;
            break;
        case TextConflictResolutionPrivate::ConflictResolutionZone::ResolveRightThenLeft:
            return rightContent + "\n" + leftContent;
            break;
    };
    return "";
}

bool TextConflictResolution::eventFilter(QObject* watched, QEvent* event) {
    if (watched == ui->resolutionArea) {
        if (event->type() == QEvent::Paint) {
            QPainter painter(ui->resolutionArea);

            auto zones = this->renderResolutionZones();
            for (int i = 0; i < zones.length(); i++) {
                auto poly = zones.at(i);
                auto zone = d->resolutionZones.at(i);

                if (poly.length() == 0) continue;

                QColor background;
                QColor border;

                if (zone.resolveDirection == TextConflictResolutionPrivate::ConflictResolutionZone::NoResolution) {
                    background = QColor(200, 0, 0, 100);
                    border = Qt::red;
                } else {
                    background = QColor(127, 127, 127, 100);
                    border = this->palette().color(QPalette::WindowText);
                }

                painter.setPen(Qt::transparent);
                painter.setBrush(background);
                painter.drawPolygon(poly);

                painter.setPen(border);
                painter.drawLine(poly.at(0), poly.at(1));
                painter.drawLine(poly.at(2), poly.at(3));
            }
        } else if (event->type() == QEvent::MouseButtonPress) {
            auto* mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() != Qt::LeftButton) return false;

            auto zones = this->renderResolutionZones();
            for (int i = 0; i < zones.length(); i++) {
                auto poly = zones.at(i);
                if (poly.containsPoint(mouseEvent->pos(), Qt::OddEvenFill)) {
                    auto* menu = new QMenu();
                    auto* leftAction = menu->addAction(tr("Take Left"), this, [=] {
                        d->resolutionZones[i].resolveDirection = TextConflictResolutionPrivate::ConflictResolutionZone::ResolveLeft;
                        this->loadResolutionZones();

                        emit conflictResolutionCompletedChanged();
                    });
                    auto* rightAction = menu->addAction(tr("Take Right"), this, [=] {
                        d->resolutionZones[i].resolveDirection = TextConflictResolutionPrivate::ConflictResolutionZone::ResolveRight;
                        this->loadResolutionZones();

                        emit conflictResolutionCompletedChanged();
                    });
                    menu->addSeparator();
                    auto* leftRightAction = menu->addAction(tr("Take Left and then Right"), this, [=] {
                        d->resolutionZones[i].resolveDirection = TextConflictResolutionPrivate::ConflictResolutionZone::ResolveLeftThenRight;
                        this->loadResolutionZones();

                        emit conflictResolutionCompletedChanged();
                    });
                    auto* rightLeftAction = menu->addAction(tr("Take Right and then Left"), this, [=] {
                        d->resolutionZones[i].resolveDirection = TextConflictResolutionPrivate::ConflictResolutionZone::ResolveRightThenLeft;
                        this->loadResolutionZones();

                        emit conflictResolutionCompletedChanged();
                    });

                    leftAction->setCheckable(true);
                    rightAction->setCheckable(true);
                    leftRightAction->setCheckable(true);
                    rightLeftAction->setCheckable(true);

                    auto actionGroup = new QActionGroup(menu);
                    actionGroup->addAction(leftAction);
                    actionGroup->addAction(rightAction);
                    actionGroup->addAction(leftRightAction);
                    actionGroup->addAction(rightLeftAction);

                    switch (d->resolutionZones.at(i).resolveDirection) {
                        case TextConflictResolutionPrivate::ConflictResolutionZone::NoResolution:
                            break;
                        case TextConflictResolutionPrivate::ConflictResolutionZone::ResolveLeft:
                            leftAction->setChecked(true);
                            break;
                        case TextConflictResolutionPrivate::ConflictResolutionZone::ResolveRight:
                            rightAction->setChecked(true);
                            break;
                        case TextConflictResolutionPrivate::ConflictResolutionZone::ResolveLeftThenRight:
                            leftRightAction->setChecked(true);
                            break;
                        case TextConflictResolutionPrivate::ConflictResolutionZone::ResolveRightThenLeft:
                            rightLeftAction->setChecked(true);
                            break;
                    }

                    connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
                    menu->popup(mouseEvent->globalPosition().toPoint());
                    return true;
                }
            }
        }
    }
    return false;
}
