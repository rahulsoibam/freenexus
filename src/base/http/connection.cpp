#include "connection.h"

#include <QRegExp>
#include <QTcpSocket>

#include "base/logger.h"
#include "irequesthandler.h"
#include "requestparser.h"
#include "responsegenerator.h"

using namespace Http;

Connection::Connection(QTcpSocket *socket, IRequestHandler *requestHandler, QObject *parent)
    : QObject(parent)
    , m_socket(socket)
    , m_requestHandler(requestHandler)
{
    m_socket->setParent(this);
    m_idleTimer.start();
    connect(m_socket, &QTcpSocket::readyRead, this, &Connection::read);
}

Connection::~Connection()
{
    m_socket->close();
}

void Connection::read()
{
    m_idleTimer.restart();
    m_receivedData.append(m_socket->readAll());

    while (!m_receivedData.isEmpty()) {
        const RequestParser::ParseResult result = RequestParser::parse(m_receivedData);

        switch (result.status) {
        case RequestParser::ParseStatus::Incomplete: {
                const long bufferLimit = RequestParser::MAX_CONTENT_SIZE * 1.1;  // some margin for headers
                if (m_receivedData.size() > bufferLimit) {
                    Logger::instance()->addMessage(tr("Http request size exceeds limiation, closing socket. Limit: %ld, IP: %s")
                        .arg(bufferLimit).arg(m_socket->peerAddress().toString()), Log::WARNING);

                    Response resp(413, "Payload Too Large");
                    resp.headers[HEADER_CONNECTION] = "close";

                    sendResponse(resp);
                    m_socket->close();
                }
            }
            return;

        case RequestParser::ParseStatus::BadRequest: {
                Logger::instance()->addMessage(tr("Bad Http request, closing socket. IP: %s")
                    .arg(m_socket->peerAddress().toString()), Log::WARNING);

                Response resp(400, "Bad Request");
                resp.headers[HEADER_CONNECTION] = "close";

                sendResponse(resp);
                m_socket->close();
            }
            return;

        case RequestParser::ParseStatus::OK: {
                const Environment env {m_socket->localAddress(), m_socket->localPort(), m_socket->peerAddress(), m_socket->peerPort()};

                Response resp = m_requestHandler->processRequest(result.request, env);

                if (acceptsGzipEncoding(result.request.headers["accept-encoding"]))
                    resp.headers[HEADER_CONTENT_ENCODING] = "gzip";

                resp.headers[HEADER_CONNECTION] = "keep-alive";

                sendResponse(resp);
                m_receivedData = m_receivedData.mid(result.frameSize);
            }
            break;

        default:
            Q_ASSERT(false);
            return;
        }
    }
}

void Connection::sendResponse(const Response &response) const
{
    m_socket->write(toByteArray(response));
}

bool Connection::hasExpired(const qint64 timeout) const
{
    return m_idleTimer.hasExpired(timeout);
}

bool Connection::isClosed() const
{
    return (m_socket->state() == QAbstractSocket::UnconnectedState);
}

bool Connection::acceptsGzipEncoding(QString codings)
{
    // [rfc7231] 5.3.4. Accept-Encoding

    const auto isCodingAvailable = [](const QStringList &list, const QString &encoding) -> bool
    {
        foreach (const QString &str, list) {
            if (!str.startsWith(encoding))
                continue;

            // without quality values
            if (str == encoding)
                return true;

            // [rfc7231] 5.3.1. Quality Values
            const QStringRef substr = str.midRef(encoding.size() + 3);  // ex. skip over "gzip;q="

            bool ok = false;
            const double qvalue = substr.toDouble(&ok);
            if (!ok || (qvalue <= 0.0))
                return false;

            return true;
        }
        return false;
    };

    const QStringList list = codings.remove(' ').remove('\t').split(',', QString::SkipEmptyParts);
    if (list.isEmpty())
        return false;

    const bool canGzip = isCodingAvailable(list, QLatin1String("gzip"));
    if (canGzip)
        return true;

    const bool canAny = isCodingAvailable(list, QLatin1String("*"));
    if (canAny)
        return true;

    return false;
}