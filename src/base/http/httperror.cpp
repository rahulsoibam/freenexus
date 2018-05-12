#include "httperror.h"

HTTPError::HTTPError(int statusCode, const QString &statusText, const QString &message)
    : RuntimeError {message}
    , m_statusCode {statusCode}
    , m_statusText {statusText}
{
}

int HTTPError::statusCode() const
{
    return m_statusCode;
}

QString HTTPError::statusText() const
{
    return m_statusText;
}

BadRequestHTTPError::BadRequestHTTPError(const QString &message)
    : HTTPError(400, QLatin1String("Bad Request"), message)
{
}

ConflictHTTPError::ConflictHTTPError(const QString &message)
    : HTTPError(409, QLatin1String("Conflict"), message)
{
}

ForbiddenHTTPError::ForbiddenHTTPError(const QString &message)
    : HTTPError(403, QLatin1String("Forbidden"), message)
{
}

NotFoundHTTPError::NotFoundHTTPError(const QString &message)
    : HTTPError(404, QLatin1String("Not Found"), message)
{
}

UnsupportedMediaTypeHTTPError::UnsupportedMediaTypeHTTPError(const QString &message)
    : HTTPError(415, QLatin1String("Unsupported Media Type"), message)
{
}

UnauthorizedHTTPError::UnauthorizedHTTPError(const QString &message)
    : HTTPError(401, QLatin1String("Unauthorized"), message)
{
}

InternalServerErrorHTTPError::InternalServerErrorHTTPError(const QString &message)
    : HTTPError(500, QLatin1String("Internal Server Error"), message)
{
}
