#ifndef HTTP_TYPES_H
#define HTTP_TYPES_H

#include <QHostAddress>
#include <QString>
#include <QVector>

#include "base/types.h"

namespace Http
{
    const char METHOD_GET[] = "GET";
    const char METHOD_POST[] = "POST";

    const char HEADER_CACHE_CONTROL[] = "cache-control";
    const char HEADER_CONNECTION[] = "connection";
    const char HEADER_CONTENT_DISPOSITION[] = "content-disposition";
    const char HEADER_CONTENT_ENCODING[] = "content-encoding";
    const char HEADER_CONTENT_LENGTH[] = "content-length";
    const char HEADER_CONTENT_SECURITY_POLICY[] = "content-security-policy";
    const char HEADER_CONTENT_TYPE[] = "content-type";
    const char HEADER_DATE[] = "date";
    const char HEADER_HOST[] = "host";
    const char HEADER_ORIGIN[] = "origin";
    const char HEADER_REFERER[] = "referer";
    const char HEADER_SET_COOKIE[] = "set-cookie";
    const char HEADER_X_CONTENT_TYPE_OPTIONS[] = "x-content-type-options";
    const char HEADER_X_FORWARDED_HOST[] = "x-forwarded-host";
    const char HEADER_X_FRAME_OPTIONS[] = "x-frame-options";
    const char HEADER_X_XSS_PROTECTION[] = "x-xss-protection";

    const char HEADER_REQUEST_METHOD_GET[] = "GET";
    const char HEADER_REQUEST_METHOD_HEAD[] = "HEAD";
    const char HEADER_REQUEST_METHOD_POST[] = "POST";

    const char CONTENT_TYPE_HTML[] = "text/html";
    const char CONTENT_TYPE_JSON[] = "application/json";
    const char CONTENT_TYPE_GIF[] = "image/gif";
    const char CONTENT_TYPE_PNG[] = "image/png";
    const char CONTENT_TYPE_TXT[] = "text/plain";
    const char CONTENT_TYPE_FORM_ENCODED[] = "application/x-www-form-urlencoded";
    const char CONTENT_TYPE_FORM_DATA[] = "multipart/form-data";

    // portability: "\r\n" doesn't guarantee mapping to the correct symbol
    const char CRLF[] = {0x0D, 0x0A, '\0'};

    struct Environment
    {
        QHostAddress localAddress;
        quint16 localPort;

        QHostAddress clientAddress;
        quint16 clientPort;
    };

    struct UploadedFile
    {
        QString filename;
        QString type;  // MIME type
        QByteArray data;
    };

    struct Request
    {
        QString version;
        QString method;
        QString path;
        QByteArray query;
        QStringMap headers;
        QStringMap posts;
        QVector<UploadedFile> files;
    };

    struct ResponseStatus
    {
        uint code;
        QString text;

        ResponseStatus(uint code = 200, const QString& text = "OK"): code(code), text(text) {}
    };

    struct Response
    {
        ResponseStatus status;
        QStringMap headers;
        QByteArray content;

        Response(uint code = 200, const QString& text = "OK"): status(code, text) {}
    };
}

#endif // HTTP_TYPES_H
