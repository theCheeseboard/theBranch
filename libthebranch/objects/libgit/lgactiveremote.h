/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2022 Victor Tran
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * *************************************/
#ifndef LGACTIVEREMOTE_H
#define LGACTIVEREMOTE_H

#include "../forward_declares.h"
#include <QCoroTask>
#include <QObject>

class git_remote;
class LGRemote;
struct LGActiveRemotePrivate;
class LGActiveRemote : public QObject,
                       public tbSharedFromThis<LGActiveRemote> {
        Q_OBJECT
    public:
        ~LGActiveRemote();

        QCoro::Task<> connect(bool isPush);
        QCoro::Task<> fetch();
        QCoro::Task<> push(QStringList refs);

        void setInformationRequiredCallback(InformationRequiredCallback callback);

    signals:
        void informationRequredResponse(QVariantMap response);

    protected:
        friend LGRemote;
        explicit LGActiveRemote(git_remote* remote, QObject* parent = nullptr);

    private:
        LGActiveRemotePrivate* d;

        QVariantMap callInformationRequiredCallback(QVariantMap params);

    private slots:
        QCoro::Task<> doCallInformationRequiredCallback(QVariantMap params);
};

#endif // LGACTIVEREMOTE_H
