#include "accountsmanager.h"

#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>

#include "github/githubaccount.h"

struct AccountsManagerPrivate {
        QString path;

        QList<AbstractAccount*> accounts;
};

AccountsManager::~AccountsManager() {
    delete d;
}

AccountsManager* AccountsManager::instance() {
    static auto* manager = new AccountsManager();
    return manager;
}

void AccountsManager::addAccount(AbstractAccount* account) {
    d->accounts.append(account);
    this->save();
    emit accountsChanged();
}

void AccountsManager::removeAccount(AbstractAccount* account) {
    d->accounts.removeAll(account);
    this->save();
    emit accountsChanged();
}

QList<AbstractAccount*> AccountsManager::accounts() {
    return d->accounts;
}

AccountsManager::AccountsManager(QObject* parent) :
    QObject{parent} {
    d = new AccountsManagerPrivate();

    QDir settingsPath(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation));
    settingsPath = settingsPath.absoluteFilePath("theSuite/theBranch");
    settingsPath.mkpath(".");
    d->path = settingsPath.absoluteFilePath("accounts.json");

    QFile file(d->path);
    file.open(QFile::ReadOnly);
    auto accounts = QJsonDocument::fromJson(file.readAll()).array();
    file.close();

    for (auto account : accounts) {
        auto obj = account.toObject();
        if (obj.value("type").toString() == "github") {
            d->accounts.append(new GitHubAccount(obj));
        }
    }
}

void AccountsManager::save() {
    QJsonArray accounts;
    for (auto* account : d->accounts) {
        accounts.append(account->save());
    }

    QFile file(d->path);
    file.open(QFile::WriteOnly);
    file.write(QJsonDocument(accounts).toJson());
    file.close();
}
