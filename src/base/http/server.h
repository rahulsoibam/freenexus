#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <QTcpServer>

#ifndef QT_NO_OPENSSL
#include <QSslCertificate>
#include <QSslCipher>
#include <QSslKey>
#endif

namespace Http
{
    class IRequestHandler;
    class Connection;

    class Server : public QTcpServer
    {
        Q_OBJECT
        Q_DISABLE_COPY(Server)

    public:
        Server(IRequestHandler *requestHandler, QObject *parent = nullptr);
        ~Server();

#ifndef QT_NO_OPENSSL
        bool setupHttps(const QByteArray &certificates, const QByteArray &key);
        void disableHttps();
#endif

    private slots:
        void dropTimedOutConnection();

    private:
        void incomingConnection(qintptr socketDescriptor);

        IRequestHandler *m_requestHandler;
        QList<Connection *> m_connections;  // for tracking persistent connections

#ifndef QT_NO_OPENSSL
        QList<QSslCipher> safeCipherList() const;

        bool m_https;
        QList<QSslCertificate> m_certificates;
        QSslKey m_key;
#endif
    };
}

#endif // HTTP_SERVER_H
