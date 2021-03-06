#include "server.h"

#include <QMutableListIterator>
#include <QNetworkProxy>
#include <QStringList>
#include <QTimer>
#ifndef QT_NO_OPENSSL
#include <QSslSocket>
#else
#include <QTcpSocket>
#endif

#include "connection.h"

static const int KEEP_ALIVE_DURATION = 7 * 1000;  // milliseconds
static const int CONNECTIONS_LIMIT = 500;
static const int CONNECTIONS_SCAN_INTERVAL = 2;  // seconds

using namespace Http;

Server::Server(IRequestHandler *requestHandler, QObject *parent)
    : QTcpServer(parent)
    , m_requestHandler(requestHandler)
#ifndef QT_NO_OPENSSL
    , m_https(false)
#endif
{
    setProxy(QNetworkProxy::NoProxy);
#ifndef QT_NO_OPENSSL
    QSslSocket::setDefaultCiphers(safeCipherList());
#endif

    QTimer *dropConnectionTimer = new QTimer(this);
    connect(dropConnectionTimer, &QTimer::timeout, this, &Server::dropTimedOutConnection);
    dropConnectionTimer->start(CONNECTIONS_SCAN_INTERVAL * 1000);
}

Server::~Server()
{
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    if (m_connections.size() >= CONNECTIONS_LIMIT) return;

    QTcpSocket *serverSocket;
#ifndef QT_NO_OPENSSL
    if (m_https)
        serverSocket = new QSslSocket(this);
    else
#endif
        serverSocket = new QTcpSocket(this);

    if (!serverSocket->setSocketDescriptor(socketDescriptor)) {
        delete serverSocket;
        return;
    }

#ifndef QT_NO_OPENSSL
    if (m_https) {
        static_cast<QSslSocket *>(serverSocket)->setProtocol(QSsl::SecureProtocols);
        static_cast<QSslSocket *>(serverSocket)->setPrivateKey(m_key);
        static_cast<QSslSocket *>(serverSocket)->setLocalCertificateChain(m_certificates);
        static_cast<QSslSocket *>(serverSocket)->setPeerVerifyMode(QSslSocket::VerifyNone);
        static_cast<QSslSocket *>(serverSocket)->startServerEncryption();
    }
#endif

    Connection *c = new Connection(serverSocket, m_requestHandler, this);
    m_connections.append(c);
}

void Server::dropTimedOutConnection()
{
    QMutableListIterator<Connection *> i(m_connections);
    while (i.hasNext()) {
        auto connection = i.next();
        if (connection->isClosed() || connection->hasExpired(KEEP_ALIVE_DURATION)) {
            delete connection;
            i.remove();
        }
    }
}

#ifndef QT_NO_OPENSSL
bool Server::setupHttps(const QByteArray &certificates, const QByteArray &key)
{
    QSslKey sslKey(key, QSsl::Rsa);
    if (sslKey.isNull())
        sslKey = QSslKey(key, QSsl::Ec);

    const QList<QSslCertificate> certs = QSslCertificate::fromData(certificates);
    const bool areCertsValid = !certs.empty() && std::all_of(certs.begin(), certs.end(), [](const QSslCertificate &c) { return !c.isNull(); });

    if (!sslKey.isNull() && areCertsValid) {
        m_key = sslKey;
        m_certificates = certs;
        m_https = true;
        return true;
    }
    else {
        disableHttps();
        return false;
    }
}

void Server::disableHttps()
{
    m_https = false;
    m_certificates.clear();
    m_key.clear();
}

QList<QSslCipher> Server::safeCipherList() const
{
    const QStringList badCiphers = {"idea", "rc4"};
    const QList<QSslCipher> allCiphers = QSslSocket::supportedCiphers();
    QList<QSslCipher> safeCiphers;
    foreach (const QSslCipher &cipher, allCiphers) {
        bool isSafe = true;
        foreach (const QString &badCipher, badCiphers) {
            if (cipher.name().contains(badCipher, Qt::CaseInsensitive)) {
                isSafe = false;
                break;
            }
        }

        if (isSafe)
            safeCiphers += cipher;
    }

    return safeCiphers;
}
#endif // QT_NO_OPENSSL
