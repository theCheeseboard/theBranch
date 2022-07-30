#ifndef ABSTRACTACCOUNT_H
#define ABSTRACTACCOUNT_H

#include <QJsonObject>
#include <QObject>

class AbstractAccount : public QObject {
        Q_OBJECT
    public:
        explicit AbstractAccount(QObject* parent = nullptr);

        virtual QString description() = 0;
        virtual QJsonObject save() = 0;
        virtual QString slugForCloneUrl(QString cloneUrl) = 0;

    signals:
};

#endif // ABSTRACTACCOUNT_H
