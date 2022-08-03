#ifndef DIFFPOPOVER_H
#define DIFFPOPOVER_H

#include "objects/diff.h"
#include <QWidget>

namespace Ui {
class DiffPopover;
}

struct DiffPopoverPrivate;
class DiffPopover : public QWidget
{
    Q_OBJECT

public:
    explicit DiffPopover(DiffPtr diff, QWidget *parent = nullptr);
    ~DiffPopover();

signals:
    void done();

private slots:
    void on_titleLabel_backButtonClicked();

private:
    Ui::DiffPopover *ui;
    DiffPopoverPrivate* d;
};

#endif // DIFFPOPOVER_H
