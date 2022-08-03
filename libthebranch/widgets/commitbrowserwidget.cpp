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
#include <tlogger.h>

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
        diff = Diff::diffTrees(d->repo, nullptr, commit->tree());
    } else if (commit->parents().length() == 1) {
        auto diffCommit = commit->parents().first();
        diff = Diff::diffTrees(d->repo, diffCommit->tree(), commit->tree());
    } else {
        // TODO
    }

    if (!diff) return;
    for (auto file : diff->diffFiles()) {
        tDebug("CommitBrowserWidget") << file.oldFilePath << " -> " << file.newFilePath;
    }
}
