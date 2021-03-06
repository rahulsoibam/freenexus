#include "downloadhandler.h"

#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkCookie>
#include <QNetworkProxy>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTemporaryFile>
#include <QUrl>

#include "base/utils/fs.h"
#include "base/utils/gzip.h"
#include "base/utils/misc.h"
#include "downloadmanager.h"

static QString errorCodeToString(QNetworkReply::NetworkError status);

using namespace Net;

DownloadHandler::DownloadHandler(QNetworkReply *reply, DownloadManager *manager, bool saveToFile, qint64 limit, bool handleRedirectToMagnet)
    : QObject(manager)
    , m_reply(reply)
    , m_manager(manager)
    , m_saveToFile(saveToFile)
    , m_sizeLimit(limit)
    , m_handleRedirectToMagnet(handleRedirectToMagnet)
    , m_url(reply->url().toString())
{
    init();
}

DownloadHandler::~DownloadHandler()
{
    if (m_reply)
        delete m_reply;
}

// Returns original url
QString DownloadHandler::url() const
{
    return m_url;
}

void DownloadHandler::processFinishedDownload()
{
    QString url = m_reply->url().toString();
    qDebug("Download finished: %s", qUtf8Printable(url));
    // Check if the request was successful
    if (m_reply->error() != QNetworkReply::NoError) {
        // Failure
        qDebug("Download failure (%s), reason: %s", qUtf8Printable(url), qUtf8Printable(errorCodeToString(m_reply->error())));
        emit downloadFailed(m_url, errorCodeToString(m_reply->error()));
        this->deleteLater();
    }
    else {
        // Check if the server ask us to redirect somewhere else
        const QVariant redirection = m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
        if (redirection.isValid()) {
            // We should redirect
            handleRedirection(redirection.toUrl());
        }
        else {
            // Success
            QByteArray replyData = m_reply->readAll();
            if (m_reply->rawHeader("Content-Encoding") == "gzip") {
                // decompress gzip reply
                replyData = Utils::Gzip::decompress(replyData);
            }

            if (m_saveToFile) {
                QString filePath;
                if (saveToFile(replyData, filePath))
                    emit downloadFinished(m_url, filePath);
                else
                    emit downloadFailed(m_url, tr("I/O Error"));
                }
            else {
                emit downloadFinished(m_url, replyData);
            }

            this->deleteLater();
        }
    }
}

void DownloadHandler::checkDownloadSize(qint64 bytesReceived, qint64 bytesTotal)
{
    QString msg = tr("The file size is %1. It exceeds the download limit of %2.");

    if (bytesTotal > 0) {
        // Total number of bytes is available
        if (bytesTotal > m_sizeLimit) {
            m_reply->abort();
            emit downloadFailed(m_url, msg.arg(Utils::Misc::friendlyUnit(bytesTotal), Utils::Misc::friendlyUnit(m_sizeLimit)));
        }
        else {
            disconnect(m_reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(checkDownloadSize(qint64, qint64)));
        }
    }
    else if (bytesReceived  > m_sizeLimit) {
        m_reply->abort();
        emit downloadFailed(m_url, msg.arg(Utils::Misc::friendlyUnit(bytesReceived), Utils::Misc::friendlyUnit(m_sizeLimit)));
    }
}

void DownloadHandler::init()
{
    m_reply->setParent(this);
    if (m_sizeLimit > 0)
        connect(m_reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(checkDownloadSize(qint64, qint64)));
    connect(m_reply, SIGNAL(finished()), this, SLOT(processFinishedDownload()));
}

bool DownloadHandler::saveToFile(const QByteArray &replyData, QString &filePath)
{
    QTemporaryFile *tmpfile = new QTemporaryFile(Utils::Fs::tempPath() + "XXXXXX");
    if (!tmpfile->open()) {
        delete tmpfile;
        return false;
    }

    tmpfile->setAutoRemove(false);
    filePath = tmpfile->fileName();
    qDebug("Temporary filename is: %s", qUtf8Printable(filePath));
    if (m_reply->isOpen() || m_reply->open(QIODevice::ReadOnly)) {
        tmpfile->write(replyData);
        tmpfile->close();
        // XXX: tmpfile needs to be deleted on Windows before using the file
        // or it will complain that the file is used by another process.
        delete tmpfile;
        return true;
    }
    else {
        delete tmpfile;
        Utils::Fs::forceRemove(filePath);
    }

    return false;
}

void DownloadHandler::handleRedirection(QUrl newUrl)
{
    // Resolve relative urls
    if (newUrl.isRelative())
        newUrl = m_reply->url().resolved(newUrl);

    const QString newUrlString = newUrl.toString();
    qDebug("Redirecting from %s to %s", qUtf8Printable(m_reply->url().toString()), qUtf8Printable(newUrlString));

    // Redirect to magnet workaround
    if (newUrlString.startsWith("magnet:", Qt::CaseInsensitive)) {
        qDebug("Magnet redirect detected.");
        m_reply->abort();
        if (m_handleRedirectToMagnet)
            emit redirectedToMagnet(m_url, newUrlString);
        else
            emit downloadFailed(m_url, tr("Unexpected redirect to magnet URI."));

        this->deleteLater();
    }
    else {
        DownloadHandler *tmp = m_manager->downloadUrl(newUrlString, m_saveToFile, m_sizeLimit, m_handleRedirectToMagnet);
        m_reply->deleteLater();
        m_reply = tmp->m_reply;
        init();
        tmp->m_reply = nullptr;
        delete tmp;
    }
}

QString errorCodeToString(QNetworkReply::NetworkError status)
{
    switch (status) {
    case QNetworkReply::HostNotFoundError:
        return QObject::tr("The remote host name was not found (invalid hostname)");
    case QNetworkReply::OperationCanceledError:
        return QObject::tr("The operation was canceled");
    case QNetworkReply::RemoteHostClosedError:
        return QObject::tr("The remote server closed the connection prematurely, before the entire reply was received and processed");
    case QNetworkReply::TimeoutError:
        return QObject::tr("The connection to the remote server timed out");
    case QNetworkReply::SslHandshakeFailedError:
        return QObject::tr("SSL/TLS handshake failed");
    case QNetworkReply::ConnectionRefusedError:
        return QObject::tr("The remote server refused the connection");
    case QNetworkReply::ProxyConnectionRefusedError:
        return QObject::tr("The connection to the proxy server was refused");
    case QNetworkReply::ProxyConnectionClosedError:
        return QObject::tr("The proxy server closed the connection prematurely");
    case QNetworkReply::ProxyNotFoundError:
        return QObject::tr("The proxy host name was not found");
    case QNetworkReply::ProxyTimeoutError:
        return QObject::tr("The connection to the proxy timed out or the proxy did not reply in time to the request sent");
    case QNetworkReply::ProxyAuthenticationRequiredError:
        return QObject::tr("The proxy requires authentication in order to honor the request but did not accept any credentials offered");
    case QNetworkReply::ContentAccessDenied:
        return QObject::tr("The access to the remote content was denied (401)");
    case QNetworkReply::ContentOperationNotPermittedError:
        return QObject::tr("The operation requested on the remote content is not permitted");
    case QNetworkReply::ContentNotFoundError:
        return QObject::tr("The remote content was not found at the server (404)");
    case QNetworkReply::AuthenticationRequiredError:
        return QObject::tr("The remote server requires authentication to serve the content but the credentials provided were not accepted");
    case QNetworkReply::ProtocolUnknownError:
        return QObject::tr("The Network Access API cannot honor the request because the protocol is not known");
    case QNetworkReply::ProtocolInvalidOperationError:
        return QObject::tr("The requested operation is invalid for this protocol");
    case QNetworkReply::UnknownNetworkError:
        return QObject::tr("An unknown network-related error was detected");
    case QNetworkReply::UnknownProxyError:
        return QObject::tr("An unknown proxy-related error was detected");
    case QNetworkReply::UnknownContentError:
        return QObject::tr("An unknown error related to the remote content was detected");
    case QNetworkReply::ProtocolFailure:
        return QObject::tr("A breakdown in protocol was detected");
    default:
        return QObject::tr("Unknown error");
    }
}
