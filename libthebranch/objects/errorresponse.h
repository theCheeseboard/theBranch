#ifndef ERRORRESPONSE_H
#define ERRORRESPONSE_H

#include <QCoreApplication>
#include <QString>
#include <QVariantMap>

#define CHK_ERR(err) ErrorResponse error = err

class ErrorResponse {
        Q_DECLARE_TR_FUNCTIONS(ErrorResponse);

    public:
        enum ErrorType {
            NoError,
            UnspecifiedError,
            UnableToPushNonFastForwardableReferenceError
        };

        ErrorResponse(ErrorType error, QString description, QVariantMap supplementaryData = {});
        ErrorResponse();

        ErrorType error();
        QString errorString();
        QString description();
        QVariantMap supplementaryData();

        operator bool();

        static ErrorResponse fromCurrentGitError();

    private:
        ErrorType _error;
        QString _description;
        QVariantMap _supplementaryData;
};

#endif // ERRORRESPONSE_H
