#include "commitbox.h"
#include "ui_commitbox.h"

#include "objects/commit.h"
#include "objects/repository.h"

struct CommitBoxPrivate {
        RepositoryPtr repo;
        CommitPtr commit;
};

CommitBox::CommitBox(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::CommitBox) {
    ui->setupUi(this);
    d = new CommitBoxPrivate();
}

CommitBox::~CommitBox() {
    delete d;
    delete ui;
}

void CommitBox::setRepository(RepositoryPtr repo) {
    d->repo = repo;
}

CommitPtr CommitBox::commit() {
    return d->commit;
}

void CommitBox::on_refBox_textChanged(const QString& arg1) {
    if (arg1.isEmpty()) {
        d->commit = nullptr;
    } else {
        d->commit = d->repo->searchCommit(arg1);
    }

    if (d->commit) {
        QStringList refInformation;
        ui->refName->setText(d->commit->commitMessage().trimmed());
        refInformation.append(tr("Commit"));
        refInformation.append(d->commit->shortCommitHash());
        refInformation.append(d->commit->authorName());
        ui->refInformation->setText(refInformation.join(libContemporaryCommon::humanReadablePartJoinString()));
    } else {
        ui->refName->setText("");
        ui->refInformation->setText("");
    }
    emit commitChanged(d->commit);
}
