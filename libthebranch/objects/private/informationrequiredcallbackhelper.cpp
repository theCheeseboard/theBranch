#include "informationrequiredcallbackhelper.h"

#include "../libgit/lgrepository.h"
#include "branchservices.h"
#include "cred/gitcredentialmanager.h"
#include <QCoroCore>

struct InformationRequiredCallbackHelperPrivate {
        InformationRequiredCallback informationRequiredCallback;
        LGRepositoryPtr repo;

        void* passthroughPayload;
};

InformationRequiredCallbackHelper::InformationRequiredCallbackHelper(QWidget* parent) :
    QWidget{parent} {
    d = new InformationRequiredCallbackHelperPrivate();
}

InformationRequiredCallbackHelper::~InformationRequiredCallbackHelper() {
    delete d;
}

void InformationRequiredCallbackHelper::setRepo(LGRepositoryPtr repo) {
    d->repo = repo;
}

void InformationRequiredCallbackHelper::setInformationRequiredCallback(InformationRequiredCallback callback) {
    d->informationRequiredCallback = callback;
}

void InformationRequiredCallbackHelper::setPassthroughPayload(void* passthroughPayload) {
    d->passthroughPayload = passthroughPayload;
}

void* InformationRequiredCallbackHelper::passthroughPayload() {
    return d->passthroughPayload;
}

git_credential_acquire_cb InformationRequiredCallbackHelper::acquireCredentialCallback() {
    return [](git_credential** out, const char* url, const char* username_from_url, uint allowed_types, void* payload) -> int {
        auto parent = reinterpret_cast<InformationRequiredCallbackHelper*>(payload);

        QVariantMap params;
        params.insert("type", "credential");
        if (parent->d->repo) params.insert("repo", QVariant::fromValue(parent->d->repo));
        params.insert("url", QString(url));
        if (allowed_types & GIT_CREDENTIAL_SSH_KEY) {
            params.insert("credType", "ssh-key");

            try {
                auto response = parent->callInformationRequiredCallback(params);
                auto pubkey = response.value("pubKey").toString().toUtf8();
                auto privKey = response.value("privKey").toString().toUtf8();
                git_credential_ssh_key_new(out, username_from_url, pubkey.data(), privKey.data(), "");
            } catch (const QException& ex) {
                return -1;
            }
        } else if (allowed_types & GIT_CREDENTIAL_USERPASS_PLAINTEXT) {
            params.insert("credType", "username-password");

            try {
                auto response = parent->callInformationRequiredCallback(params);
                auto username = response.value("username").toString().toUtf8();
                auto password = response.value("password").toString().toUtf8();
                git_credential_userpass_plaintext_new(out, username.data(), password.data());
            } catch (const QException& ex) {
                return -1;
            }
        } else {
            return GIT_PASSTHROUGH;
        }
        return 0;
    };
}

git_transport_certificate_check_cb InformationRequiredCallbackHelper::certificateCheckCallback() {
    return [](git_cert* cert, int valid, const char* host, void* payload) -> int {
        auto parent = reinterpret_cast<InformationRequiredCallbackHelper*>(payload);

        auto params = BranchServices::cred()->certParams(cert, host);
        auto trust = BranchServices::cred()->checkCert(params);
        if (trust == GitCredentialManager::GitCertCheckResult::Passed) {
            return 0;
        }

        if (trust == GitCredentialManager::GitCertCheckResult::UnexpectedCert) params.insert("unexpected", true);
        if (valid) return 0;

        try {
            if (parent->d->repo) params.insert("repo", QVariant::fromValue(parent->d->repo));
            parent->callInformationRequiredCallback(params);
            params.remove("repo");

            BranchServices::cred()->trustCert(params);
            return 0;
        } catch (const QException& ex) {
            return -1;
        }
    };
}

QVariantMap InformationRequiredCallbackHelper::callInformationRequiredCallback(QVariantMap params) {
    QMetaObject::invokeMethod(this, "doCallInformationRequiredCallback", Qt::QueuedConnection, Q_ARG(QVariantMap, params));
    auto response = QCoro::waitFor(qCoro(this, &InformationRequiredCallbackHelper::informationRequredResponse));
    if (response.isEmpty()) throw QException();
    return response;
}

QCoro::Task<> InformationRequiredCallbackHelper::doCallInformationRequiredCallback(QVariantMap params) {
    try {
        auto response = co_await d->informationRequiredCallback(params);
        emit informationRequredResponse(response);
    } catch (QException ex) {
        emit informationRequredResponse({});
    }
}
