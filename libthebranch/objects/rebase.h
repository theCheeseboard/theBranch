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
#ifndef REBASE_H
#define REBASE_H

#include "gitoperation.h"
#include "libthebranch_global.h"
#include <QObject>

struct RebasePrivate;
class LIBTHEBRANCH_EXPORT Rebase : public GitOperation {
        Q_OBJECT
    public:
        explicit Rebase(RepositoryPtr repo, BranchPtr from, BranchPtr onto, QObject* parent = nullptr);
        ~Rebase();

        BranchPtr from();
        BranchPtr onto();

        void setSignature(LGSignaturePtr signature);

        bool isValid();
        int operationCount();
        int currentOperation();

    signals:
        void conflictEncountered();
        void rebaseComplete();

    private:
        RebasePrivate* d;

        // GitOperation interface
    public:
        RepositoryPtr repository();
        void abortOperation();
        void finaliseOperation();
};

class PullRebase : public Rebase {
        Q_OBJECT
    public:
        explicit PullRebase(RepositoryPtr repo, BranchPtr from, BranchPtr onto, QObject* parent = nullptr);
};

#endif // REBASE_H
