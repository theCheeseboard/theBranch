#include "errorresponse.h"

#include <QCoreApplication>
#include <git2.h>

ErrorResponse::ErrorResponse(ErrorType error, QString description, QVariantMap supplementaryData) {
    _error = error;
    _description = description;
    _supplementaryData = supplementaryData;
}

ErrorResponse::ErrorResponse() {
    _error = NoError;
}

ErrorResponse::ErrorType ErrorResponse::error() {
    return _error;
}

QString ErrorResponse::errorString() {
    switch (_error) {
        case ErrorResponse::NoError:
            return tr("No Error");
        case ErrorResponse::UnspecifiedError:
            return tr("Unspecified Error");
            break;
    }
}

QString ErrorResponse::description() {
    return _description;
}

QVariantMap ErrorResponse::supplementaryData() {
    return _supplementaryData;
}

ErrorResponse::operator bool() {
    return _error != ErrorResponse::NoError;
}

ErrorResponse ErrorResponse::fromCurrentGitError() {
    const git_error* err = git_error_last();
    if (err == nullptr) return ErrorResponse();
    return ErrorResponse(UnspecifiedError, QString::fromUtf8(err->message));
}
