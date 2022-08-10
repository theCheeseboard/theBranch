#ifndef LGCONFIG_H
#define LGCONFIG_H

#include <QObject>
#include "../forward_declares.h"

struct git_config;
struct LGConfigPrivate;
class LGConfig : public QObject, public tbSharedFromThis<LGConfig> {
        Q_OBJECT
    public:
        explicit LGConfig(git_config* gitConfig, bool isSnapshot, QObject* parent = nullptr);
        ~LGConfig();

        QString getString(QString setting);
        void setString(QString setting, QString value);

        bool getBool(QString setting);
        void setBool(QString setting, bool value);

        void deleteSetting(QString setting);

        static LGConfigPtr defaultConfig();
        LGConfigPtr snapshot();

    signals:

    private:
        struct LGConfigPrivate* d;

};

#endif // LGCONFIG_H
