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

#include "../errorresponse.h"
#include "lgrepository.h"
#include <QCoroFuture>
#include <QCoroSignal>
#include <QtConcurrent>
#include <git2.h>

struct LGActiveRemotePrivate {
        git_remote* remote;
        git_remote_callbacks callbacks;

        InformationRequiredCallback informationRequiredCallback;
};

LGActiveRemote::LGActiveRemote(git_remote* remote, QObject* parent) :
    QObject{parent} {
    d = new LGActiveRemotePrivate();
    d->remote = remote;

    d->callbacks = GIT_REMOTE_CALLBACKS_INIT;
    d->callbacks.payload = this;
    d->callbacks.credentials = [](git_credential** out, const char* url, const char* username_from_url, uint allowed_types, void* payload) -> int {
        auto parent = reinterpret_cast<LGActiveRemote*>(payload);

        QVariantMap params;
        params.insert("type", "credential");
        params.insert("url", QString(url));
        if (allowed_types & GIT_CREDENTIAL_SSH_KEY) {
            params.insert("credType", "ssh_key");

            try {
                auto response = parent->callInformationRequiredCallback(params);
                auto pubkey = response.value("pubKey").toString().toUtf8();
                auto privKey = response.value("privKey").toString().toUtf8();
                git_credential_ssh_key_new(out, username_from_url, pubkey.data(), privKey.data(), "");
            } catch (const QException& ex) {
                return -1;
            }
        } else {
            return GIT_PASSTHROUGH;
        }
        return 0;
    };
    d->callbacks.certificate_check = [](git_cert* cert, int valid, const char* host, void* payload) -> int {
        auto parent = reinterpret_cast<LGActiveRemote*>(payload);

        QVariantMap params;
        params.insert("type", "certcheck");
        params.insert("host", QString(host));

        if (valid) return 0;
        switch (cert->cert_type) {
            case GIT_CERT_NONE:
                return -1;
            case GIT_CERT_X509:
                {
                    return -1;
                    break;
                }
            case GIT_CERT_HOSTKEY_LIBSSH2:
                {
                    auto* hostKeyCert = reinterpret_cast<git_cert_hostkey*>(cert);
                    params.insert("certType", "ssh");
                    if (hostKeyCert->type & GIT_CERT_SSH_MD5) params.insert("hash-md5", QString(QByteArray::fromRawData(reinterpret_cast<const char*>(hostKeyCert->hash_md5), sizeof(hostKeyCert->hash_md5)).toHex()));
                    if (hostKeyCert->type & GIT_CERT_SSH_SHA1) params.insert("hash-sha1", QString(QByteArray::fromRawData(reinterpret_cast<const char*>(hostKeyCert->hash_sha1), sizeof(hostKeyCert->hash_sha1)).toHex()));
                    if (hostKeyCert->type & GIT_CERT_SSH_SHA256) params.insert("hash-sha256", QString(QByteArray::fromRawData(reinterpret_cast<const char*>(hostKeyCert->hash_sha256), sizeof(hostKeyCert->hash_sha256)).toHex()));
                    if (hostKeyCert->type & GIT_CERT_SSH_RAW) {
                        params.insert("hostkey", QString(QByteArray::fromRawData(reinterpret_cast<const char*>(hostKeyCert->hostkey), sizeof(hostKeyCert->hostkey_len)).toHex()));

                        QString type;
                        switch (hostKeyCert->raw_type) {
                            case GIT_CERT_SSH_RAW_TYPE_UNKNOWN:
                                type = tr("Unknown");
                                break;
                            case GIT_CERT_SSH_RAW_TYPE_RSA:
                                type = "RSA";
                                break;
                            case GIT_CERT_SSH_RAW_TYPE_DSS:
                                type = "DSS";
                                break;
                            case GIT_CERT_SSH_RAW_TYPE_KEY_ECDSA_256:
                                type = "ECDSA256";
                                break;
                            case GIT_CERT_SSH_RAW_TYPE_KEY_ECDSA_384:
                                type = "ECDSA384";
                                break;
                            case GIT_CERT_SSH_RAW_TYPE_KEY_ECDSA_521:
                                type = "ECDSA521";
                                break;
                            case GIT_CERT_SSH_RAW_TYPE_KEY_ED25519:
                                type = "ED25519";
                                break;
                        }
                        params.insert("hostkeyType", type);
                    }
                    break;
                }
            case GIT_CERT_STRARRAY:
                {
                    return -1;
                    break;
                }
        }

        try {
            parent->callInformationRequiredCallback(params);
            return 0;
        } catch (const QException& ex) {
            return -1;
        }
    };
}

QVariantMap LGActiveRemote::callInformationRequiredCallback(QVariantMap params) {
    QMetaObject::invokeMethod(this, "doCallInformationRequiredCallback", Qt::QueuedConnection, Q_ARG(QVariantMap, params));
    auto response = QCoro::waitFor(qCoro(this, &LGActiveRemote::informationRequredResponse));
    if (response.isEmpty()) throw QException();
    return response;
}

QCoro::Task<> LGActiveRemote::doCallInformationRequiredCallback(QVariantMap params) {
    try {
        auto response = co_await d->informationRequiredCallback(params);
        emit informationRequredResponse(response);
    } catch (QException ex) {
        emit informationRequredResponse({});
    }
}

LGActiveRemote::~LGActiveRemote() {
    git_remote_free(d->remote);
    delete d;
}

QCoro::Task<> LGActiveRemote::connect(bool isPush) {
    auto callbacks = &d->callbacks;
    auto error = co_await QtConcurrent::run([this, isPush, callbacks] {
        git_proxy_options proxyOptions = GIT_PROXY_OPTIONS_INIT;
        git_strarray customHeaders;
        customHeaders.count = 0;
        git_remote_connect(d->remote, isPush ? GIT_DIRECTION_PUSH : GIT_DIRECTION_FETCH, callbacks, &proxyOptions, &customHeaders);
        return ErrorResponse::fromCurrentGitError();
    });
    error.throwIfError();
}

QCoro::Task<> LGActiveRemote::fetch() {
    auto callbacks = &d->callbacks;
    auto error = co_await QtConcurrent::run([this, callbacks] {
        git_remote_fetch(d->remote, nullptr, nullptr, nullptr);
        return ErrorResponse::fromCurrentGitError();
    });
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

        git_remote_push(d->remote, &refsToPush, &pushOptions);

        for (auto i = 0; i < refs.length(); i++) {
            free(strings[i]);
        }
        delete[] strings;
        return ErrorResponse::fromCurrentGitError();
    },
        refs, d->callbacks);
    error.throwIfError();
}

void LGActiveRemote::setInformationRequiredCallback(InformationRequiredCallback callback) {
    d->informationRequiredCallback = callback;
}
