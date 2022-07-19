#ifndef USERNAMEPASSWORDSNAPIN_H
#define USERNAMEPASSWORDSNAPIN_H

#include "informationrequestsnapin.h"
#include "objects/forward_declares.h"
#include <QWidget>

namespace Ui {
    class UsernamePasswordSnapIn;
}

struct UsernamePasswordSnapInPrivate;
class UsernamePasswordSnapIn : public InformationRequestSnapIn {
        Q_OBJECT

    public:
        explicit UsernamePasswordSnapIn(QVariantMap params, QWidget* parent = nullptr);
        ~UsernamePasswordSnapIn();

    private slots:
        void on_titleLabel_backButtonClicked();

        void on_authenticateButton_clicked();

    private:
        Ui::UsernamePasswordSnapIn* ui;
        UsernamePasswordSnapInPrivate* d;
};

#endif // USERNAMEPASSWORDSNAPIN_H
