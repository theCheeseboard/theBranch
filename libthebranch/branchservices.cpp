#include "branchservices.h"

#include "accounts/accountsmanager.h"
#include "cred/gitcredentialmanager.h"

struct BranchServicesPrivate {
        AccountsManager* accounts;
        GitCredentialManager* cred;
};

BranchServices::~BranchServices() {
    delete d;
}

BranchServices* BranchServices::instance() {
    static auto instance = new BranchServices();
    return instance;
}

AccountsManager* BranchServices::accounts() {
    return instance()->d->accounts;
}

GitCredentialManager* BranchServices::cred() {
    return instance()->d->cred;
}

BranchServices::BranchServices(QObject* parent) :
    QObject{parent} {
    d = new BranchServicesPrivate();
    d->accounts = new AccountsManager(this);
    d->cred = new GitCredentialManager(this);
}
