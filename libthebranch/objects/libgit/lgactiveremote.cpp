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
#include "lgactiveremote.h"

#include "../private/informationrequiredcallbackhelper.h"
#include "branchservices.h"
#include "lgrepository.h"
#include <QCoroFuture>
#include <QCoroSignal>
#include <QtConcurrent>
#include <git2.h>

struct LGActiveRemotePrivate {
        git_remote* remote;
        git_remote_callbacks callbacks;

        LGRepositoryPtr repo;

        InformationRequiredCallbackHelper informationRequiredCallbackHelper;
};

LGActiveRemote::LGActiveRemote(git_remote* remote, LGRepositoryPtr repo, QObject* parent) :
    QObject{parent} {
    d = new LGActiveRemotePrivate();
    d->remote = remote;
    d->repo = repo;

    // Ensure cred manager is available
    BranchServices::cred();

    d->informationRequiredCallbackHelper.setPassthroughPayload(this);

    d->callbacks = GIT_REMOTE_CALLBACKS_INIT;
    d->callbacks.payload = &d->informationRequiredCallbackHelper;
    d->callbacks.credentials = d->informationRequiredCallbackHelper.acquireCredentialCallback();
    d->callbacks.certificate_check = d->informationRequiredCallbackHelper.certificateCheckCallback();
}

LGActiveRemote::~LGActiveRemote() {
    git_remote_free(d->remote);
    delete d;
}

LGActiveRemotePtr LGActiveRemote::fromDetached(QString url) {
    git_remote* remote;
    if (git_remote_create_detached(&remote, url.toUtf8().data()) != 0) {
        return nullptr;
    }

    return (new LGActiveRemote(remote, nullptr))->sharedFromThis();
}

QCoro::Task<> LGActiveRemote::connect(bool isPush) {
    auto callbacks = &d->callbacks;
    auto error = co_await QtConcurrent::run([this, isPush, callbacks] {
        git_proxy_options proxyOptions = GIT_PROXY_OPTIONS_INIT;
        git_strarray customHeaders;
        customHeaders.count = 0;
        auto success = git_remote_connect(d->remote, isPush ? GIT_DIRECTION_PUSH : GIT_DIRECTION_FETCH, callbacks, &proxyOptions, &customHeaders) == 0;
        if (success) return ErrorResponse();
        auto error = ErrorResponse::fromCurrentGitError();
        if (error.error() == ErrorResponse::NoError) {
            return ErrorResponse(ErrorResponse::UnspecifiedError, tr("Unable to connect to remote"));
        }
        return error;
    });
    error.throwIfError();
}

QCoro::Task<> LGActiveRemote::fetch(QStringList refs) {
    auto callbacks = &d->callbacks;
    auto error = co_await QtConcurrent::run([this, callbacks](QStringList refs) {
        git_strarray refsToFetch;
        refsToFetch.count = refs.length();
        auto strings = new char*[refs.length()];
        for (auto i = 0; i < refs.length(); i++) {
            strings[i] = strdup(refs.at(i).toUtf8().data());
        }
        refsToFetch.strings = strings;

        auto success = git_remote_fetch(d->remote, refs.isEmpty() ? nullptr : &refsToFetch, nullptr, nullptr) == 0;

        for (auto i = 0; i < refs.length(); i++) {
            free(strings[i]);
        }
        delete[] strings;

        if (success) return ErrorResponse();
        return ErrorResponse::fromCurrentGitError();
    },
        refs);
    error.throwIfError();
}

QCoro::Task<> LGActiveRemote::push(QStringList refs) {
    auto error = co_await QtConcurrent::run([this](QStringList refs, git_remote_callbacks callbacks) {
        git_push_options pushOptions = GIT_PUSH_OPTIONS_INIT;
        pushOptions.callbacks = callbacks;

        git_strarray refsToPush;
        refsToPush.count = refs.length();
        auto strings = new char*[refs.length()];
        for (auto i = 0; i < refs.length(); i++) {
            strings[i] = strdup(refs.at(i).toUtf8().data());
        }
        refsToPush.strings = strings;

        auto success = git_remote_push(d->remote, &refsToPush, &pushOptions) == 0;

        for (auto i = 0; i < refs.length(); i++) {
            free(strings[i]);
        }
        delete[] strings;

        if (success) return ErrorResponse();
        return ErrorResponse::fromCurrentGitError();
    },
        refs, d->callbacks);
    error.throwIfError();
}

QCoro::Task<QStringList> LGActiveRemote::fetchRefspecs() {
    struct ReturnValue {
            ErrorResponse error;
            QStringList refs;
    };

    auto callbacks = &d->callbacks;
    auto retval = co_await QtConcurrent::run([this, callbacks]() -> ReturnValue {
        git_strarray refs;

        auto success = git_remote_get_fetch_refspecs(&refs, d->remote) == 0;

        if (!success) {
            return ReturnValue{ErrorResponse::fromCurrentGitError(), {}};
        }

        QStringList refList;
        for (auto i = 0; i < refs.count; i++) {
            refList.append(QByteArray(refs.strings[i]));
        }
        return ReturnValue{ErrorResponse(), refList};
    });
    retval.error.throwIfError();
    co_return retval.refs;
}

void LGActiveRemote::setInformationRequiredCallback(InformationRequiredCallback callback) {
    d->informationRequiredCallbackHelper.setInformationRequiredCallback(callback);
}
