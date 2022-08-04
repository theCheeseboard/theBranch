#ifndef GITCREDENTIALMANAGER_H
#define GITCREDENTIALMANAGER_H

#include "../libthebranch_global.h"
#include <QObject>

struct git_cert;
class BranchServices;
struct GitCredentialManagerPrivate;
class LIBTHEBRANCH_EXPORT GitCredentialManager : public QObject {
        Q_OBJECT
    public:
        ~GitCredentialManager();

        enum class GitCertCheckResult {
            Passed,
            NoEntry,
            UnexpectedCert
        };

        QVariantMap certParams(git_cert* cert, QString host);
        GitCertCheckResult checkCert(QVariantMap params);
        void trustCert(QVariantMap params);

        void save();

    signals:

    protected:
        friend BranchServices;
        explicit GitCredentialManager(QObject* parent = nullptr);

    private:
        GitCredentialManagerPrivate* d;
};

#endif // GITCREDENTIALMANAGER_H
