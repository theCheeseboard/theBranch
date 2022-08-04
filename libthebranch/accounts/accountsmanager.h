#ifndef ACCOUNTSMANAGER_H
#define ACCOUNTSMANAGER_H

#include <QObject>

class AbstractAccount;
class BranchServices;
struct AccountsManagerPrivate;
class AccountsManager : public QObject {
        Q_OBJECT
    public:
        ~AccountsManager();

        void addAccount(AbstractAccount* account);
        void removeAccount(AbstractAccount* account);

        QList<AbstractAccount*> accounts();

    protected:
        friend BranchServices;
        explicit AccountsManager(QObject* parent = nullptr);

    signals:
        void accountsChanged();

    private:
        AccountsManagerPrivate* d;

        void save();
};

#endif // ACCOUNTSMANAGER_H
