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
#include "remote.h"

#include "libgit/lgremote.h"
#include "libgit/lgrepository.h"

struct RemotePrivate {
        LGRemotePtr remote;
        LGRepositoryPtr repo;
};

Remote::~Remote() {
    delete d;
}

QString Remote::name() {
    return d->remote->name();
}

QString Remote::url() {
    return d->remote->url();
}

QCoro::Task<> Remote::fetch(QStringList refs, InformationRequiredCallback callback) {
    co_await d->repo->fetch(d->remote->name(), refs, callback);
}

void Remote::remove() {
    d->remote->remove();
}

RemotePtr Remote::remoteForLgRemote(LGRepositoryPtr repo, LGRemotePtr remote) {
    Remote* r = new Remote();
    r->d->remote = remote;
    r->d->repo = repo;
    return r->sharedFromThis();
}

Remote::Remote(QObject* parent) :
    QObject{parent} {
    d = new RemotePrivate();
}
