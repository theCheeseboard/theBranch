#include "diffpopover.h"
#include "ui_diffpopover.h"

#include "objects/diff.h"
#include "objects/statusitemlistmodel.h"

#include <merge/textmergetool.h>

struct DiffPopoverPrivate {
        DiffPtr diff;
        StatusItemListModel* statusModel;

        QMap<QString, QWidget*> diffWidgets;
};

DiffPopover::DiffPopover(DiffPtr diff, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::DiffPopover) {
    ui->setupUi(this);

    d = new DiffPopoverPrivate();
    d->diff = diff;

    auto diffResult = diff->diffFiles();

    d->statusModel = new StatusItemListModel(this);
    d->statusModel->setDiffResult(diffResult);
    d->statusModel->setUserCheckable(false);

    ui->diffList->setModel(d->statusModel);
    ui->diffList->setItemDelegate(new StatusItemListDelegate(this));

    ui->leftWidget->setFixedWidth(SC_DPI_W(300, this));
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::Lift);

    for (auto i = 0; i < d->statusModel->rowCount(); i++) {
        auto index = d->statusModel->index(i, 0);
        auto path = index.data(StatusItemListModel::PathRole).toString();
        auto oldSide = index.data(StatusItemListModel::BlobOldSide).toByteArray();
        auto newSide = index.data(StatusItemListModel::BlobNewSide).toByteArray();
        QWidget* resolutionWidget;

        // TODO: Choose an appropriate widget
        auto *textMergeTool = new TextMergeTool(this);
        textMergeTool->setReadOnly(true);
        textMergeTool->loadDiff(oldSide, newSide);
        textMergeTool->setTitles(diff->oldSideDescription(), diff->newSideDescription());
        resolutionWidget = textMergeTool;

        ui->stackedWidget->addWidget(resolutionWidget);
        d->diffWidgets.insert(path, resolutionWidget);
    }

    connect(ui->diffList->selectionModel(), &QItemSelectionModel::currentRowChanged, this, [=](QModelIndex current) {
        if (current.isValid()) {
            ui->stackedWidget->setCurrentWidget(d->diffWidgets.value(current.data(StatusItemListModel::PathRole).toString()));
        }
    });
}

DiffPopover::~DiffPopover() {
    delete d;
    delete ui;
}

void DiffPopover::on_titleLabel_backButtonClicked() {
    emit done();
}
