#include "newtagpopover.h"
#include "ui_newtagpopover.h"

#include "objects/repository.h"
#include "objects/tag.h"
#include <tcontentsizer.h>

struct NewTagPopoverPrivate {
        RepositoryPtr repo;
        ICommitResolvablePtr commit;
};

NewTagPopover::NewTagPopover(RepositoryPtr repo, ICommitResolvablePtr commit, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::NewTagPopover) {
    ui->setupUi(this);
    d = new NewTagPopoverPrivate();
    d->repo = repo;
    d->commit = commit;

    ui->titleLabel->setBackButtonShown(true);
    new tContentSizer(ui->tagOptionsWidget);
    new tContentSizer(ui->tagButton);
    new tContentSizer(ui->statusFrame);

    ui->statusFrame->setState(tStatusFrame::Warning);
    ui->statusFrame->setTitle(tr("Heads up!"));
    ui->statusFrame->setText(tr("There is already a tag with that name in the repository. Creating a tag will move the existing tag to point to this commit."));
    ui->statusFrame->setVisible(false);
}

NewTagPopover::~NewTagPopover() {
    delete ui;
}

void NewTagPopover::on_tagButton_clicked() {
    d->repo->createLightweightTag(ui->nameBox->text(), d->commit);
    emit done();
}

void NewTagPopover::on_titleLabel_backButtonClicked() {
    emit done();
}

void NewTagPopover::on_nameBox_textChanged(const QString& arg1) {
    for (auto tag : d->repo->tags()) {
        if (tag->name() == arg1) {
            ui->statusFrame->setVisible(true);
            return;
        }
    }
    ui->statusFrame->setVisible(false);
}
