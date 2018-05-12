#pragma once

#include "base/exceptions.h"

class HTTPError : public RuntimeError
{
public:
    HTTPError(int statusCode, const QString &statusText, const QString &message = "");

    int statusCode() const;
    QString statusText() const;

private:
    const int m_statusCode;
    const QString m_statusText;
};

class BadRequestHTTPError : public HTTPError
{
public:
    explicit BadRequestHTTPError(const QString &message = "");
};

class ForbiddenHTTPError : public HTTPError
{
public:
    explicit ForbiddenHTTPError(const QString &message = "");
};

class NotFoundHTTPError : public HTTPError
{
public:
    explicit NotFoundHTTPError(const QString &message = "");
};

class ConflictHTTPError : public HTTPError
{
public:
    explicit ConflictHTTPError(const QString &message = "");
};

class UnsupportedMediaTypeHTTPError : public HTTPError
{
public:
    explicit UnsupportedMediaTypeHTTPError(const QString &message = "");
};

class UnauthorizedHTTPError : public HTTPError
{
public:
    explicit UnauthorizedHTTPError(const QString &message = "");
};

class InternalServerErrorHTTPError : public HTTPError
{
public:
    explicit InternalServerErrorHTTPError(const QString &message = "");
};
