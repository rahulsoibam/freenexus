#include "responsegenerator.h"

#include <QDateTime>

#include "base/utils/gzip.h"

QByteArray Http::toByteArray(Response response)
{
    compressContent(response);

    response.headers[HEADER_CONTENT_LENGTH] = QString::number(response.content.length());
    response.headers[HEADER_DATE] = httpDate();

    QByteArray buf;
    buf.reserve(10 * 1024);

    // Status Line
    buf += QString("HTTP/%1 %2 %3")
        .arg("1.1",  // TODO: depends on request
            QString::number(response.status.code),
            response.status.text)
        .toLatin1()
        .append(CRLF);

    // Header Fields
    for (auto i = response.headers.constBegin(); i != response.headers.constEnd(); ++i)
        buf += QString("%1: %2").arg(i.key(), i.value()).toLatin1().append(CRLF);

    // the first empty line
    buf += CRLF;

    // message body  // TODO: support HEAD request
    buf += response.content;

    return buf;
}

QString Http::httpDate()
{
    // [RFC 7231] 7.1.1.1. Date/Time Formats
    // example: "Sun, 06 Nov 1994 08:49:37 GMT"

    return QLocale::c().toString(QDateTime::currentDateTimeUtc(), QLatin1String("ddd, dd MMM yyyy HH:mm:ss"))
        .append(QLatin1String(" GMT"));
}

void Http::compressContent(Response &response)
{
    if (response.headers.value(HEADER_CONTENT_ENCODING) != QLatin1String("gzip"))
        return;

    response.headers.remove(HEADER_CONTENT_ENCODING);

    // for very small files, compressing them only wastes cpu cycles
    const int contentSize = response.content.size();
    if (contentSize <= 1024)  // 1 kb
        return;

    // filter out known hard-to-compress types
    const QString contentType = response.headers[HEADER_CONTENT_TYPE];
    if ((contentType == CONTENT_TYPE_GIF) || (contentType == CONTENT_TYPE_PNG))
        return;

    // try compressing
    bool ok = false;
    const QByteArray compressedData = Utils::Gzip::compress(response.content, 6, &ok);
    if (!ok)
        return;

    // "Content-Encoding: gzip\r\n" is 24 bytes long
    if ((compressedData.size() + 24) >= contentSize)
        return;

    response.content = compressedData;
    response.headers[HEADER_CONTENT_ENCODING] = QLatin1String("gzip");
}
