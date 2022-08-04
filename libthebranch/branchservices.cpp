#include "branchservices.h"

#include "accounts/accountsmanager.h"

struct BranchServicesPrivate {
        AccountsManager* accounts;
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

BranchServices::BranchServices(QObject* parent) :
    QObject{parent} {
    d = new BranchServicesPrivate();
    d->accounts = new AccountsManager(this);
}
