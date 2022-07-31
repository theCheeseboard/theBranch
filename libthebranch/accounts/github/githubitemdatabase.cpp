#include "githubitemdatabase.h"

#include "githubitem.h"
#include <QJsonObject>
#include <QMap>
#include <QSharedPointer>

GitHubItemDatabase::GitHubItemDatabase(QObject* parent) :
    QObject{parent} {
}

GitHubItemDatabase::~GitHubItemDatabase() {
}

GitHubItemPtr GitHubItemDatabase::item(QString nodeId) {
    return this->items.value(nodeId);
}
