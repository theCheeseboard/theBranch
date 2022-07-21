#ifndef ERRORRESPONSE_H
#define ERRORRESPONSE_H

#include <QCoreApplication>
#include <QException>
#include <QString>
#include <QVariantMap>

#define CHK_ERR(err) ErrorResponse error = err

class GitRepositoryOutOfDateException : public QException {
    public:
        void raise() const override;
        GitRepositoryOutOfDateException* clone() const override;
};

class GitException : public QException {
    public:
        GitException(QString description);

        void raise() const override;
        GitException* clone() const override;

        QString description() const;

    private:
        QString _description;
};

class ErrorResponse {
        Q_DECLARE_TR_FUNCTIONS(ErrorResponse);

    public:
        enum ErrorType {
            NoError,
            UnspecifiedError,
            UnableToPushNonFastForwardableReferenceError,
            WorkingDirectoryNotCleanError
        };

        ErrorResponse(ErrorType error, QString description, QVariantMap supplementaryData = {});
        ErrorResponse();

        ErrorType error();
        QString errorString();
        QString description();
        QVariantMap supplementaryData();

        void throwIfError();

        operator bool();

        static ErrorResponse fromCurrentGitError();

    private:
        ErrorType _error;
        QString _description;
        QVariantMap _supplementaryData;
};

#endif // ERRORRESPONSE_H
