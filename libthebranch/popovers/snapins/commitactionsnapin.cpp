#include "commitactionsnapin.h"
#include "ui_commitactionsnapin.h"

#include "../snapinpopover.h"
#include "cherrypicksnapin.h"
#include "objects/cherrypick.h"
#include "objects/commit.h"
#include "objects/revert.h"
#include "revertsnapin.h"
#include <tcontentsizer.h>
#include <ticon.h>

struct CommitActionSnapInPrivate {
        RepositoryPtr repo;
        CommitActionSnapIn::CommitAction action;
};

CommitActionSnapIn::CommitActionSnapIn(RepositoryPtr repo, CommitAction action, QWidget* parent) :
    SnapIn(parent),
    ui(new Ui::CommitActionSnapIn) {
    ui->setupUi(this);
    d = new CommitActionSnapInPrivate();
    d->repo = repo;
    d->action = action;

    new tContentSizer(ui->optionsWidget);
    new tContentSizer(ui->performActionButton);

    switch (action) {
        case CommitAction::CherryPick:
            ui->titleLabel->setText(tr("Cherry Pick"));
            ui->optionsLabel->setText(tr("Cherry Pick Options"));
            ui->commitLabel->setText(tr("Cherry Pick:"));
            ui->performActionButton->setText(tr("Cherry Pick"));
            ui->performActionButton->setIcon(tIcon::fromTheme("vcs-cherry-pick"));
            break;
        case CommitAction::Revert:
            ui->titleLabel->setText(tr("Revert"));
            ui->optionsLabel->setText(tr("Revert Options"));
            ui->commitLabel->setText(tr("Revert:"));
            ui->performActionButton->setText(tr("Revert"));
            ui->performActionButton->setIcon(tIcon::fromTheme("vcs-revert"));
            break;
    }

    connect(ui->commitBox, &CommitBox::commitChanged, this, &CommitActionSnapIn::updateWidgets);
    ui->commitBox->setRepository(repo);

    updateWidgets();
}

CommitActionSnapIn::~CommitActionSnapIn() {
    delete ui;
    delete d;
}

void CommitActionSnapIn::on_titleLabel_backButtonClicked() {
    emit done();
}

void CommitActionSnapIn::updateWidgets() {
    ui->performActionButton->setEnabled(!ui->commitBox->commit().isNull());
}

void CommitActionSnapIn::on_performActionButton_clicked() {
    switch (d->action) {
        case CommitAction::CherryPick:
            {
                CherryPickPtr cherryPick(new CherryPick(d->repo, ui->commitBox->commit()));
                this->parentPopover()->pushSnapIn(new CherryPickSnapIn(cherryPick));
                emit done();
                break;
            }
        case CommitAction::Revert:
            {
                RevertPtr revert(new Revert(d->repo, ui->commitBox->commit()));
                this->parentPopover()->pushSnapIn(new RevertSnapIn(revert));
                emit done();
                break;
            }
    }
}
