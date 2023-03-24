#ifndef COMMITBOX_H
#define COMMITBOX_H

#include "objects/forward_declares.h"
#include <QWidget>

namespace Ui {
    class CommitBox;
}

struct CommitBoxPrivate;
class CommitBox : public QWidget {
        Q_OBJECT

    public:
        explicit CommitBox(QWidget* parent = nullptr);
        ~CommitBox();

        void setRepository(RepositoryPtr repo);

        CommitPtr commit();

    signals:
        void commitChanged(CommitPtr ref);

    private slots:
        void on_refBox_textChanged(const QString& arg1);

    private:
        Ui::CommitBox* ui;
        CommitBoxPrivate* d;
};

#endif // COMMITBOX_H
