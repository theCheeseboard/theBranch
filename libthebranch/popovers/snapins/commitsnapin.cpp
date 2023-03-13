#include "commitsnapin.h"
#include "popovers/snapinpopover.h"
#include "pushsnapin.h"
#include "ui_commitsnapin.h"

#include "objects/blob.h"
#include "objects/commit.h"
#include "objects/libgit/lgcommit.h"
#include "objects/libgit/lgindex.h"
#include "objects/libgit/lgreference.h"
#include "objects/libgit/lgrepository.h"
#include "objects/libgit/lgsignature.h"
#include "objects/reference.h"
#include "objects/statusitemlistmodel.h"
#include "objects/tree.h"
#include <QMenu>
#include <tcontentsizer.h>
#include <terrorflash.h>

struct CommitSnapInPrivate {
        RepositoryPtr repository;
        StatusItemListModel* statusModel;
        StatusItemListFilterView* statusModelFilter;

        LGSignaturePtr signature;

        static int StandardFlagFilters;
};

int CommitSnapInPrivate::StandardFlagFilters = Repository::StatusItem::Modified | Repository::StatusItem::Renamed | Repository::StatusItem::Deleted | Repository::StatusItem::Conflicting;

CommitSnapIn::CommitSnapIn(RepositoryPtr repository, QWidget* parent) :
    SnapIn(parent),
    ui(new Ui::CommitSnapIn) {
    ui->setupUi(this);
    d = new CommitSnapInPrivate();
    d->repository = repository;

    ui->titleLabel->setBackButtonShown(true);
    ui->titleLabel_2->setBackButtonShown(true);
    ui->leftWidget->setFixedWidth(SC_DPI_W(300, this));
    libContemporaryCommon::fixateHeight(ui->commitMessageEdit, [=] {
        return SC_DPI_W(100, this);
    });
    ui->textDiffPage->setReadOnly(true);
    new tContentSizer(ui->signatureWidget);

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);

    d->statusModel = new StatusItemListModel(this);
    connect(d->repository.data(), &Repository::repositoryUpdated, this, [=] {
        d->statusModel->setStatusItems(repository->fileStatus());
    });
    d->statusModel->setStatusItems(repository->fileStatus());

    d->statusModelFilter = new StatusItemListFilterView(this);
    d->statusModelFilter->setSourceModel(d->statusModel);
    d->statusModelFilter->setFlagFilters(CommitSnapInPrivate::StandardFlagFilters);
    ui->modifiedFilesEdit->setModel(d->statusModelFilter);
    ui->modifiedFilesEdit->setItemDelegate(new StatusItemListDelegate(this));

    connect(d->statusModel, &StatusItemListModel::dataChanged, this, [this] {
        d->statusModelFilter->invalidate();
        this->updateState();
    });
    connect(d->statusModel, &StatusItemListModel::checkedItemsChanged, this, [this] {
        this->updateState();
        int count = d->statusModel->checkedItems().count();
        ui->commitButton->setText(tr("Commit %n Files", nullptr, count));
        ui->commitButton->setEnabled(count != 0);
    });

    int count = d->statusModel->checkedItems().count();
    ui->commitButton->setText(tr("Commit %n Files", nullptr, count));
    ui->commitButton->setEnabled(count != 0);

    connect(ui->modifiedFilesEdit->selectionModel(), &QItemSelectionModel::selectionChanged, this, &CommitSnapIn::updateSelection);

    if (!d->repository->head()) {
        // Ensure that if the HEAD is unborn we are always viewing untracked files (because nothing would be tracked yet)
        ui->viewUntrackedCheckbox->setChecked(true);
        ui->viewUntrackedCheckbox->setEnabled(false);
    } else {
        if (!d->repository->head()->asBranch()) {
            ui->commitWarning->setTitle(tr("HEAD is detached"));
            ui->commitWarning->setText(tr("You are not currently committing to a branch. If this is not what you intended, you should checkout the branch that you wanted to commit to first."));
            ui->commitWarning->setState(tStatusFrame::Warning);
        } else {
            ui->commitWarning->setVisible(false);
            ui->commitWarningLine->setVisible(false);
        }
    }

    this->updateState();
    this->updateSelection();
}

CommitSnapIn::~CommitSnapIn() {
    delete d;
    delete ui;
}

void CommitSnapIn::on_titleLabel_backButtonClicked() {
    emit done();
}

void CommitSnapIn::on_commitButton_clicked() {
    if (ui->commitMessageEdit->toPlainText().isEmpty()) {
        tErrorFlash::flashError(ui->commitMessageEdit);
        return;
    }

    LGRepositoryPtr repo = d->repository->git_repository();
    LGSignaturePtr sig = repo->defaultSignature();
    if (!sig) {
        ui->stackedWidget->setCurrentWidget(ui->finaliseCommitPage);
    } else {
        d->signature = sig;
        performCommit();
    }
}

void CommitSnapIn::performCommit() {
    auto repo = d->repository;
    auto sig = d->signature;
    auto head = repo->head();

    // Create a commit on the existing HEAD
    auto index = repo->git_repository()->index();

    if (head) {
        // Reset the index to the state of the tree
        index->readTree(head->asCommit()->tree()->gitTree());
    }

    for (auto selected : d->statusModel->checkedItems()) {
        auto pathspec = selected.data(StatusItemListModel::PathRole).toString();
        auto flags = selected.data(StatusItemListModel::StatusRole).value<Repository::StatusItem::StatusFlag>();
        if (flags & Repository::StatusItem::Deleted) {
            if (!index->removeByPath(pathspec)) {
                ui->stackedWidget->setCurrentWidget(ui->commitPage);
                auto err = ErrorResponse::fromCurrentGitError();
                tErrorFlash::flashError(ui->commitMessageEdit, err.description());
                return;
            }
        } else {
            if (!index->addByPath(pathspec)) {
                ui->stackedWidget->setCurrentWidget(ui->commitPage);
                auto err = ErrorResponse::fromCurrentGitError();
                tErrorFlash::flashError(ui->commitMessageEdit, err.description());
                return;
            }
        }
    }

    if (!index->write()) {
        ui->stackedWidget->setCurrentWidget(ui->commitPage);
        ErrorResponse err = ErrorResponse::fromCurrentGitError();
        tErrorFlash::flashError(ui->commitMessageEdit, err.description());
        return;
    }

    auto treeOid = repo->git_repository()->index()->writeTree(repo->git_repository());
    auto tree = repo->git_repository()->lookupTree(treeOid);

    QList<LGCommitPtr> parents;
    if (head) {
        // Only connect the parent commit if the HEAD is not unborn
        parents.append(head->asCommit()->gitCommit());
    }

    auto commitOid = repo->git_repository()->createCommit(sig, sig, ui->commitMessageEdit->toPlainText(), tree, parents);
    if (!commitOid) {
        ui->stackedWidget->setCurrentWidget(ui->commitPage);
        auto error = ErrorResponse::fromCurrentGitError();
        tErrorFlash::flashError(ui->commitMessageEdit, error.description());
        return;
    }

    if (ui->pushBox->isChecked()) {
        this->parentPopover()->pushSnapIn(new PushSnapIn(d->repository));
    }
    emit done();
}

void CommitSnapIn::updateState() {
    bool haveSelected = false;
    bool haveUnselected = false;

    auto count = 0;
    for (auto i = 0; i < d->statusModel->rowCount(); i++) {
        if (d->statusModelFilter->acceptRowWithFlags(CommitSnapInPrivate::StandardFlagFilters, i, QModelIndex())) {
            count++;
            auto checkState = d->statusModel->index(i).data(Qt::CheckStateRole).value<Qt::CheckState>();
            if (checkState == Qt::Checked) haveSelected = true;
            if (checkState == Qt::Unchecked) haveUnselected = true;
        }
    }
    ui->selectAllModifiedCheckbox->setText(tr("Select %n modified files", nullptr, count));
    ui->selectAllModifiedCheckbox->setEnabled(count != 0);

    if (haveSelected && haveUnselected) {
        ui->selectAllModifiedCheckbox->setCheckState(Qt::PartiallyChecked);
    } else if (haveSelected) {
        ui->selectAllModifiedCheckbox->setCheckState(Qt::Checked);
    } else {
        ui->selectAllModifiedCheckbox->setCheckState(Qt::Unchecked);
    }
}

void CommitSnapIn::updateSelection() {
    auto selection = ui->modifiedFilesEdit->selectionModel()->selectedIndexes();
    if (selection.isEmpty()) {
        ui->diffStack->setCurrentWidget(ui->noFilePage);
    } else {
        auto first = selection.first();
        auto path = first.data(StatusItemListModel::PathRole).toString();

        QFile file(QDir(d->repository->repositoryPath()).filePath(path));
        file.open(QFile::ReadOnly);
        auto localChanges = file.readAll();
        file.close();

        auto head = d->repository->head();
        if (head) {
            auto commit = head->asCommit();
            auto tree = commit->tree();
            auto blob = tree->blobForPath(path);

            ui->textDiffPage->setTitles(commit->shortCommitHash(), tr("Local Changes"));
            if (blob) {
                ui->textDiffPage->loadDiff(blob->contents(), localChanges);
            } else {
                // File is untracked
                ui->textDiffPage->loadDiff("", localChanges);
            }
        } else {
            ui->textDiffPage->setTitles(tr("New Repository"), tr("Local Changes"));
            ui->textDiffPage->loadDiff("", localChanges);
        }

        ui->diffStack->setCurrentWidget(ui->textDiffPage);
    }
}

void CommitSnapIn::on_titleLabel_2_backButtonClicked() {
    ui->stackedWidget->setCurrentWidget(ui->commitPage);
}

void CommitSnapIn::on_commitButton_2_clicked() {
    if (ui->nameBox->text().isEmpty()) {
        tErrorFlash::flashError(ui->nameBox);
        return;
    }

    if (ui->emailBox->text().isEmpty()) {
        tErrorFlash::flashError(ui->emailBox);
        return;
    }

    if (ui->saveSignatureBox->isChecked()) {
        // TODO: Save the user information in the Git config
    }

    LGRepositoryPtr repo = d->repository->git_repository();
    d->signature = LGSignature::signatureForNow(ui->nameBox->text(), ui->emailBox->text());
    performCommit();
}

void CommitSnapIn::on_promoteLabel_linkActivated(const QString& link) {
}

void CommitSnapIn::on_selectAllModifiedCheckbox_stateChanged(int arg1) {
    if (arg1 == Qt::PartiallyChecked) return;

    for (auto i = 0; i < d->statusModel->rowCount(); i++) {
        if (d->statusModelFilter->acceptRowWithFlags(CommitSnapInPrivate::StandardFlagFilters, i, QModelIndex())) {
            d->statusModel->setData(d->statusModel->index(i, 0), arg1, Qt::CheckStateRole);
        }
    }

    ui->selectAllModifiedCheckbox->setTristate(false);
}

void CommitSnapIn::on_viewUntrackedCheckbox_toggled(bool checked) {
    int flagFilters = CommitSnapInPrivate::StandardFlagFilters;
    if (checked) flagFilters |= Repository::StatusItem::New;
    d->statusModelFilter->setFlagFilters(flagFilters);
}

void CommitSnapIn::on_modifiedFilesEdit_customContextMenuRequested(const QPoint& pos) {
    auto selected = ui->modifiedFilesEdit->selectionModel()->selectedIndexes();
    if (selected.isEmpty()) return;

    auto tree = d->repository->head()->asCommit()->tree();

    QStringList filesToRevert;
    QStringList filesToDelete;
    for (auto index : selected) {
        auto path = index.data(StatusItemListModel::PathRole).toString();
        auto blob = tree->blobForPath(path);
        if (blob) {
            filesToRevert.append(path);
        } else {
            filesToDelete.append(path);
        }
    }

    auto performRevert = [filesToRevert, filesToDelete, this] {
        for (const auto& filename : filesToRevert) {
            d->repository->resetFileToHead(filename);
        }

        for (const auto& filename : filesToDelete) {
            d->repository->resetFileToHead(filename);
        }

        d->statusModel->setStatusItems(d->repository->fileStatus());
    };

    auto* menu = new QMenu();

    if (!filesToDelete.isEmpty() && !filesToRevert.isEmpty()) {
        menu->addSection(tr("For %n files", nullptr, filesToDelete.count() + filesToRevert.count()));
        menu->addAction(QIcon::fromTheme("edit-undo"), tr("Discard Changes and Delete New Files"), this, performRevert);
    } else if (filesToDelete.isEmpty()) {
        menu->addSection(tr("For %n files", nullptr, filesToDelete.count() + filesToRevert.count()));
        menu->addAction(QIcon::fromTheme("edit-undo"), tr("Discard Changes"), this, performRevert);
    } else {
        menu->addSection(tr("For %n untracked files", nullptr, filesToDelete.count() + filesToRevert.count()));
        menu->addAction(QIcon::fromTheme("edit-delete"), tr("Delete from Repository"), this, performRevert);
    }

    menu->popup(ui->modifiedFilesEdit->mapToGlobal(pos));
}
