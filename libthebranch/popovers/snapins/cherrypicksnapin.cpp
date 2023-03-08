#include "cherrypicksnapin.h"
#include "ui_cherrypicksnapin.h"

#include "../snapinpopover.h"
#include "conflictresolutionsnapin.h"
#include "objects/cherrypick.h"
#include "objects/commit.h"
#include "objects/reference.h"
#include "objects/repository.h"
#include <QButtonGroup>
#include <QRadioButton>
#include <tcontentsizer.h>

struct CherryPickSnapInPrivate {
        CherryPickPtr cherryPick;
        QButtonGroup* buttonGroup;
};

CherryPickSnapIn::CherryPickSnapIn(CherryPickPtr cherryPick, QWidget* parent) :
    SnapIn(parent),
    ui(new Ui::CherryPickSnapIn) {
    ui->setupUi(this);

    d = new CherryPickSnapInPrivate();
    d->cherryPick = cherryPick;
    d->buttonGroup = new QButtonGroup(this);

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
    ui->titleLabel->setBackButtonShown(true);
    ui->titleLabel_2->setBackButtonShown(true);
    new tContentSizer(ui->cherryPickOptionsWidget);
    new tContentSizer(ui->cherryPickButton);
    new tContentSizer(ui->cherryPickUnavailableWidget);
    new tContentSizer(ui->cherryPickUnavailableButton);

    if (cherryPick->commit()->parents().count() > 1) {
        ui->cherryPickSubtitle->setText(tr("Cherry pick merge commit"));
        for (auto commit : cherryPick->commit()->parents()) {
            auto* button = new QRadioButton();
            button->setText(QStringLiteral("%1: %2").arg(commit->shortCommitHash(), commit->commitMessage().trimmed()));
            connect(button, &QRadioButton::toggled, this, [cherryPick, commit, this](bool checked) {
                if (checked) {
                    cherryPick->setMainlineCommit(commit);
                    ui->cherryPickButton->setEnabled(true);
                }
            });
            ui->mainlineLayout->addWidget(button);
            d->buttonGroup->addButton(button);
        }
        ui->cherryPickButton->setEnabled(false);
    } else {
        ui->cherryPickMergeWidget->setVisible(false);
        ui->cherryPickSubtitle->setText(tr("Cherry pick commit?"));
    }

    QString headBranch = "HEAD";
    if (cherryPick->repository()->head()->asBranch()) {
        headBranch = cherryPick->repository()->head()->asBranch()->name();
    }
    ui->cherryPickDescription->setText(tr("Do you want to cherry-pick %1 onto %2?").arg(QLocale().quoteString(cherryPick->commit()->shortCommitHash()), QLocale().quoteString(headBranch)));
    ui->cherryPickInformation->setText(tr("%1 will be copied to %2.").arg(QLocale().quoteString(cherryPick->commit()->shortCommitHash()), QLocale().quoteString(headBranch)));
}

CherryPickSnapIn::~CherryPickSnapIn() {
    delete d;
    delete ui;
}

void CherryPickSnapIn::on_cherryPickButton_clicked() {
    auto result = d->cherryPick->performCherryPick();
    if (result == CherryPick::CherryPickFailed) {
        ErrorResponse error = d->cherryPick->cherryPickFailureReason();

        QStringList conflicts = error.supplementaryData().value("conflicts").toStringList();

        if (conflicts.length() > 0) {
            ui->cherryPickUnavailableTitle->setText(tr("Unclean Working Directory"));
            ui->cherryPickUnavailableText->setText(tr("To perform a cherry pick, you need to stash your uncommitted changes first."));
            return;
        } else {
            ui->cherryPickUnavailableTitle->setText(tr("Cherry Pick failed"));
            ui->cherryPickUnavailableText->setText(error.description());
        }

        ui->stackedWidget->setCurrentWidget(ui->cherryPickUnavailablePage);
    } else if (result == CherryPick::CherryPickConflict) {
        this->parentPopover()->pushSnapIn(new ConflictResolutionSnapIn(d->cherryPick));
        emit done();
    } else {
        emit done();
    }
}

void CherryPickSnapIn::on_titleLabel_backButtonClicked() {
    emit done();
}

void CherryPickSnapIn::on_cherryPickUnavailableButton_clicked() {
    emit done();
}

void CherryPickSnapIn::on_titleLabel_2_backButtonClicked() {
    ui->stackedWidget->setCurrentWidget(ui->cherryPickPage);
}
