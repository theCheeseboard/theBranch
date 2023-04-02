#ifndef INFORMATIONREQUIREDCALLBACKHELPER_H
#define INFORMATIONREQUIREDCALLBACKHELPER_H

#include "../forward_declares.h"
#include <QWidget>
#include <git2.h>

struct InformationRequiredCallbackHelperPrivate;
class InformationRequiredCallbackHelper : public QWidget {
        Q_OBJECT
    public:
        explicit InformationRequiredCallbackHelper(QWidget* parent = nullptr);
        ~InformationRequiredCallbackHelper();

        void setRepo(LGRepositoryPtr repo);
        void setInformationRequiredCallback(InformationRequiredCallback callback);
        void setPassthroughPayload(void* passthroughPayload);

        void* passthroughPayload();

        git_credential_acquire_cb acquireCredentialCallback();
        git_transport_certificate_check_cb certificateCheckCallback();

    signals:
        void informationRequredResponse(QVariantMap response);

    private:
        InformationRequiredCallbackHelperPrivate* d;

        QVariantMap callInformationRequiredCallback(QVariantMap params);

    private slots:
        QCoro::Task<> doCallInformationRequiredCallback(QVariantMap params);
};

#endif // INFORMATIONREQUIREDCALLBACKHELPER_H
