#include "revertsnapin.h"
#include "ui_revertsnapin.h"

#include "../snapinpopover.h"
#include "conflictresolutionsnapin.h"
#include "objects/commit.h"
#include "objects/revert.h"
#include <QButtonGroup>
#include <QRadioButton>
#include <tcontentsizer.h>

struct RevertSnapInPrivate {
        RevertPtr revert;
        QButtonGroup* buttonGroup;
};

RevertSnapIn::RevertSnapIn(RevertPtr revert, QWidget* parent) :
    SnapIn(parent),
    ui(new Ui::RevertSnapIn) {
    ui->setupUi(this);

    d = new RevertSnapInPrivate();
    d->revert = revert;
    d->buttonGroup = new QButtonGroup(this);

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
    ui->titleLabel->setBackButtonShown(true);
    ui->titleLabel_2->setBackButtonShown(true);
    new tContentSizer(ui->revertOptionsWidget);
    new tContentSizer(ui->revertButton);
    new tContentSizer(ui->revertUnavailableWidget);
    new tContentSizer(ui->revertUnavailableButton);

    if (revert->commit()->parents().count() > 1) {
        ui->revertSubtitle->setText(tr("Revert merge commit"));
        for (auto commit : revert->commit()->parents()) {
            auto* button = new QRadioButton();
            button->setText(QStringLiteral("%1: %2").arg(commit->shortCommitHash(), commit->commitMessage().trimmed()));
            connect(button, &QRadioButton::toggled, this, [revert, commit, this](bool checked) {
                if (checked) {
                    revert->setMainlineCommit(commit);
                    ui->revertButton->setEnabled(true);
                }
            });
            ui->mainlineLayout->addWidget(button);
            d->buttonGroup->addButton(button);
        }
        ui->revertButton->setEnabled(false);
    } else {
        ui->revertMergeWidget->setVisible(false);
        ui->revertSubtitle->setText(tr("Revert commit?"));
    }

    ui->revertDescription->setText(tr("Do you want to revert %1?").arg(QLocale().quoteString(revert->commit()->shortCommitHash())));
    ui->revertInformation->setText(tr("A new commit that undoes changes introduced in %1 will be created.").arg(QLocale().quoteString(revert->commit()->shortCommitHash())));
}

RevertSnapIn::~RevertSnapIn() {
    delete ui;
}

void RevertSnapIn::on_revertButton_clicked() {
    auto result = d->revert->performRevert();
    if (result == Revert::RevertFailed) {
        ErrorResponse error = d->revert->revertFailureReason();

        QStringList conflicts = error.supplementaryData().value("conflicts").toStringList();

        if (conflicts.length() > 0) {
            ui->revertUnavailableTitle->setText(tr("Unclean Working Directory"));
            ui->revertUnavailableText->setText(tr("To perform a revert, you need to stash your uncommitted changes first."));
            return;
        } else {
            ui->revertUnavailableTitle->setText(tr("Revert failed"));
            ui->revertUnavailableText->setText(error.description());
        }

        ui->stackedWidget->setCurrentWidget(ui->revertUnavailablePage);
    } else if (result == Revert::RevertConflict) {
        this->parentPopover()->pushSnapIn(new ConflictResolutionSnapIn(d->revert));
        emit done();
    } else {
        emit done();
    }
}

void RevertSnapIn::on_titleLabel_backButtonClicked() {
    emit done();
}

void RevertSnapIn::on_revertUnavailableButton_clicked() {
    emit done();
}

void RevertSnapIn::on_titleLabel_2_backButtonClicked() {
    ui->stackedWidget->setCurrentWidget(ui->revertPage);
}
