#ifndef ACCOUNTSMANAGER_H
#define ACCOUNTSMANAGER_H

#include <QObject>

class AbstractAccount;
struct AccountsManagerPrivate;
class AccountsManager : public QObject {
        Q_OBJECT
    public:
        ~AccountsManager();

        static AccountsManager* instance();

        void addAccount(AbstractAccount* account);
        void removeAccount(AbstractAccount* account);

        QList<AbstractAccount*> accounts();

    signals:
        void accountsChanged();

    private:
        explicit AccountsManager(QObject* parent = nullptr);
        AccountsManagerPrivate* d;

        void save();
};

#endif // ACCOUNTSMANAGER_H
