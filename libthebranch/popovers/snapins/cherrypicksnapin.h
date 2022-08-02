#ifndef CHERRYPICKSNAPIN_H
#define CHERRYPICKSNAPIN_H

#include "../../libthebranch_global.h"
#include "objects/forward_declares.h"
#include "snapin.h"

namespace Ui {
    class CherryPickSnapIn;
}

struct CherryPickSnapInPrivate;
class LIBTHEBRANCH_EXPORT CherryPickSnapIn : public SnapIn {
        Q_OBJECT

    public:
        explicit CherryPickSnapIn(CherryPickPtr cherryPick, QWidget* parent = nullptr);
        ~CherryPickSnapIn();

    private slots:
        void on_cherryPickButton_clicked();

        void on_titleLabel_backButtonClicked();

        void on_cherryPickUnavailableButton_clicked();

        void on_titleLabel_2_backButtonClicked();

    private:
        Ui::CherryPickSnapIn* ui;
        CherryPickSnapInPrivate* d;
};

#endif // CHERRYPICKSNAPIN_H
