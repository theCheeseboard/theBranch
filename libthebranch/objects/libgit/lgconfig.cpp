#include "lgconfig.h"

#include <git2.h>

struct LGConfigPrivate {
    git_config* config;
    bool isSnapshot = false;
};

LGConfig::LGConfig(git_config* gitConfig, bool isSnapshot, QObject* parent)
    : QObject{parent} {
    d = new LGConfigPrivate();
    d->config = gitConfig;
    d->isSnapshot = isSnapshot;
}

LGConfig::~LGConfig() {
    git_config_free(d->config);
    delete d;
}

QString LGConfig::getString(QString setting) {
    if (!d->isSnapshot) return this->snapshot()->getString(setting);

    const char* value;
    if (git_config_get_string(&value, d->config, setting.toUtf8().data()) != 0) return QString();
    return QString(value);
}

void LGConfig::setString(QString setting, QString value) {
    git_config_set_string(d->config, setting.toUtf8().data(), value.toUtf8().data());
}

bool LGConfig::getBool(QString setting) {
    if (!d->isSnapshot) return this->snapshot()->getBool(setting);

    int value;
    if (git_config_get_bool(&value, d->config, setting.toUtf8().data()) != 0) return false;
    return value != 0;
}

void LGConfig::setBool(QString setting, bool value) {
    git_config_set_bool(d->config, setting.toUtf8().data(), value);
}

void LGConfig::deleteSetting(QString setting) {
    git_config_delete_entry(d->config, setting.toUtf8().data());
}

LGConfigPtr LGConfig::defaultConfig() {
    git_config* defaultConfig;
    if (git_config_open_default(&defaultConfig) != 0) return nullptr;
    return (new LGConfig(defaultConfig, false))->sharedFromThis();
}

LGConfigPtr LGConfig::snapshot() {
    git_config* snapshot;
    if (git_config_snapshot(&snapshot, d->config) != 0) return nullptr;
    return (new LGConfig(snapshot, true))->sharedFromThis();
}
