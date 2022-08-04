#include "gitcredentialmanager.h"

#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <git2.h>

struct GitCredentialManagerPrivate {
        QMap<QString, QVariantMap> certParams;
        QString path;
};

GitCredentialManager::~GitCredentialManager() {
    delete d;
}

QVariantMap GitCredentialManager::certParams(git_cert* cert, QString host) {
    QVariantMap params;
    params.insert("type", "certcheck");
    params.insert("host", QString(host));

    switch (cert->cert_type) {
        case GIT_CERT_NONE:
            return {};
        case GIT_CERT_X509:
            {
                return {};
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
                return {};
            }
    }
    return params;
}

GitCredentialManager::GitCertCheckResult GitCredentialManager::checkCert(QVariantMap params) {
    auto host = params.value("host").toString();
    if (!d->certParams.contains(host)) return GitCertCheckResult::NoEntry;

    auto param = d->certParams.value(host);
    if (param == params) return GitCertCheckResult::Passed;

    return GitCertCheckResult::UnexpectedCert;
}

void GitCredentialManager::trustCert(QVariantMap params) {
    d->certParams.insert(params.value("host").toString(), params);
    this->save();
}

void GitCredentialManager::save() {
    QJsonObject root;

    QJsonObject certs;
    for (const auto& cert : d->certParams.keys()) {
        certs.insert(cert, QJsonObject::fromVariantMap(d->certParams.value(cert)));
    }
    root.insert("certs", certs);

    QFile file(d->path);
    file.open(QFile::WriteOnly);
    file.write(QJsonDocument(root).toJson());
    file.close();
}

GitCredentialManager::GitCredentialManager(QObject* parent) :
    QObject{parent} {
    d = new GitCredentialManagerPrivate();

    QDir settingsPath(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation));
    settingsPath = settingsPath.absoluteFilePath("theSuite/theBranch");
    settingsPath.mkpath(".");
    d->path = settingsPath.absoluteFilePath("cred.json");

    QFile file(d->path);
    file.open(QFile::ReadOnly);
    auto root = QJsonDocument::fromJson(file.readAll()).object();
    file.close();

    auto certs = root.value("certs").toObject();
    for (auto cert : certs.keys()) {
        d->certParams.insert(cert, certs.value(cert).toObject().toVariantMap());
    }
}
