/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2022 Victor Tran
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * *************************************/
#include "commitbrowserwidget.h"
#include "ui_commitbrowserwidget.h"

#include "objects/commit.h"
#include "objects/commitmodel.h"
#include "objects/diff.h"
#include "popovers/diffpopover.h"
#include <tlogger.h>
#include <tpopover.h>

struct CommitBrowserWidgetPrivate {
        RepositoryPtr repo;
};

CommitBrowserWidget::CommitBrowserWidget(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::CommitBrowserWidget) {
    ui->setupUi(this);

    d = new CommitBrowserWidgetPrivate();
}

CommitBrowserWidget::~CommitBrowserWidget() {
    delete d;
    delete ui;
}

void CommitBrowserWidget::setRepository(RepositoryPtr repo) {
    d->repo = repo;
    ui->listView->setRepository(repo);
}

void CommitBrowserWidget::setStartBranch(BranchPtr branch) {
    ui->listView->setStartBranch(branch);
}

void CommitBrowserWidget::on_listView_clicked(const QModelIndex& index) {
    auto commit = index.data(CommitModel::Commit).value<CommitPtr>();
    DiffPtr diff;
    if (commit->parents().isEmpty()) {
        diff = Diff::diffCommits(d->repo, nullptr, commit);
    } else if (commit->parents().length() == 1) {
        auto diffCommit = commit->parents().first();
        diff = Diff::diffCommits(d->repo, diffCommit, commit);
    } else {
        // TODO
    }

    if (!diff) return;

    auto* jp = new DiffPopover(diff);
    tPopover* popover = new tPopover(jp);
    popover->setPopoverWidth(SC_DPI_W(-200, this));
    popover->setPopoverSide(tPopover::Bottom);
    connect(jp, &DiffPopover::done, popover, &tPopover::dismiss);
    connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
    connect(popover, &tPopover::dismissed, jp, &DiffPopover::deleteLater);
    popover->show(this->window());
}
