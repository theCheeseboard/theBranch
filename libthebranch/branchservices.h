#ifndef BRANCHSERVICES_H
#define BRANCHSERVICES_H

#include "libthebranch_global.h"
#include <QObject>

class GitCredentialManager;
class AccountsManager;
struct BranchServicesPrivate;
class LIBTHEBRANCH_EXPORT BranchServices : public QObject {
        Q_OBJECT
    public:
        ~BranchServices();

        static BranchServices* instance();

        static AccountsManager* accounts();
        static GitCredentialManager* cred();

    signals:

    private:
        explicit BranchServices(QObject* parent = nullptr);

        BranchServicesPrivate* d;
};

#endif // BRANCHSERVICES_H
