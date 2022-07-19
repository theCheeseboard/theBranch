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
        case ErrorResponse::UnableToPushNonFastForwardableReferenceError:
            return tr("Unable to push non-fast-forwardable reference");
    }
}

QString ErrorResponse::description() {
    return _description;
}

QVariantMap ErrorResponse::supplementaryData() {
    return _supplementaryData;
}

void ErrorResponse::throwIfError() {
    switch (this->error()) {
        case ErrorResponse::NoError:
            return;
        case ErrorResponse::UnableToPushNonFastForwardableReferenceError:
            throw GitRepositoryOutOfDateException();
        default:
            throw GitException(_description);
    }
}

ErrorResponse::operator bool() {
    return _error != ErrorResponse::NoError;
}

ErrorResponse ErrorResponse::fromCurrentGitError() {
    const git_error* err = git_error_last();
    if (err == nullptr) return ErrorResponse();

    ErrorResponse::ErrorType errorType = UnspecifiedError;
    auto message = QString::fromUtf8(err->message);
    if (message == "cannot push non-fastforwardable reference") {
        errorType = UnableToPushNonFastForwardableReferenceError;
    }
    return ErrorResponse(errorType, message);
}

void GitRepositoryOutOfDateException::raise() const {
    throw *this;
}

GitRepositoryOutOfDateException* GitRepositoryOutOfDateException::clone() const {
    return new GitRepositoryOutOfDateException(*this);
}

GitException::GitException(QString description) {
    this->_description = description;
}

void GitException::raise() const {
    throw *this;
}

GitException* GitException::clone() const {
    return new GitException(*this);
}

QString GitException::description() const {
    return _description;
}
