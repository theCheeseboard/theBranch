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
#ifndef BRANCHUIHELPER_H
#define BRANCHUIHELPER_H

#include "../libthebranch_global.h"
#include "forward_declares.h"
#include "repository.h"
#include <QCoroTask>

class QMenu;

class LIBTHEBRANCH_EXPORT BranchUiHelper : public QObject {
        Q_OBJECT

    public:
        BranchUiHelper(QObject* parent) = delete;

        static void appendCommitMenu(QMenu* menu, CommitPtr commit, RepositoryPtr repo, QWidget* parent);
        static void appendBranchMenu(QMenu* menu, BranchPtr branch, RepositoryPtr repo, QWidget* parent);
        static void appendStashMenu(QMenu* menu, StashPtr stash, RepositoryPtr repo, QWidget* parent);
        static void appendRemoteMenu(QMenu* menu, RemotePtr remote, RepositoryPtr repo, QWidget* parent);

        static void checkoutBranch(RepositoryPtr repo, BranchPtr branch, QWidget* parent);
        static QCoro::Task<> checkoutCommit(RepositoryPtr repo, CommitPtr commit, QWidget* parent);
        static void merge(RepositoryPtr repo, BranchPtr branch, QWidget* parent);
        static void cherryPick(RepositoryPtr repo, CommitPtr commit, QWidget* parent);
        static QCoro::Task<> reset(RepositoryPtr repo, CommitPtr commit, Repository::ResetType resetType, QWidget* parent);
        static QCoro::Task<> rebaseBranch(RepositoryPtr repo, BranchPtr from, BranchPtr onto, QWidget* parent);
        static QCoro::Task<> renameBranch(RepositoryPtr repo, BranchPtr branch, QWidget* parent);

        static void branch(RepositoryPtr repo, CommitPtr commit, QWidget* parent);
        static QCoro::Task<> deleteBranch(RepositoryPtr repo, BranchPtr branch, QWidget* parent);

        static QCoro::Task<> discardRepositoryChanges(RepositoryPtr repo, QWidget* parent);
};

#endif // BRANCHUIHELPER_H
