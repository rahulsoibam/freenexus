#ifndef HTTP_REQUESTPARSER_H
#define HTTP_REQUESTPARSER_H

#include "types.h"

namespace Http
{
    class RequestParser
    {
    public:
        enum class ParseStatus
        {
            OK,
            Incomplete,
            BadRequest
        };

        struct ParseResult
        {
            // when `status != ParseStatus::OK`, `request` & `frameSize` are undefined
            ParseStatus status;
            Request request;
            long frameSize;  // http request frame size (bytes)
        };

        static ParseResult parse(const QByteArray &data);

        static const long MAX_CONTENT_SIZE = 64 * 1024 * 1024;  // 64 MB

    private:
        RequestParser();

        ParseResult doParse(const QByteArray &data);
        bool parseStartLines(const QString &data);
        bool parseRequestLine(const QString &line);

        bool parsePostMessage(const QByteArray &data);
        bool parseFormData(const QByteArray &data);

        Request m_request;
    };
}

#endif // HTTP_REQUESTPARSER_H
