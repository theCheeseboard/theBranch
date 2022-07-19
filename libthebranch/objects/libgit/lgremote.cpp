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
#include "lgremote.h"

#include "lgactiveremote.h"
#include "lgrepository.h"
#include <git2.h>

struct LGRemotePrivate {
        QString remoteName;
        LGRepositoryPtr repo;
};

LGRemote::LGRemote(QString remoteName, LGRepositoryPtr repo, QObject* parent) :
    QObject{parent} {
    d = new LGRemotePrivate();
    d->remoteName = remoteName;
    d->repo = repo;
}

LGRemote::~LGRemote() {
    delete d;
}

LGActiveRemotePtr LGRemote::activeRemote() {
    git_remote* remote;
    if (git_remote_lookup(&remote, d->repo->gitRepository(), d->remoteName.toUtf8().data()) != 0) {
        return nullptr;
    }
    return (new LGActiveRemote(remote))->sharedFromThis();
}

QString LGRemote::name() {
    return d->remoteName;
}

QString LGRemote::url() {
    git_remote* remote;
    if (git_remote_lookup(&remote, d->repo->gitRepository(), d->remoteName.toUtf8().data()) != 0) {
        return "";
    }

    QString url = git_remote_url(remote);
    git_remote_free(remote);
    return url;
}

void LGRemote::remove() {
    git_remote_delete(d->repo->gitRepository(), d->remoteName.toUtf8().data());
}
