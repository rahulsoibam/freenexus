#ifndef HTTP_CONNECTION_H
#define HTTP_CONNECTION_H

#include <QElapsedTimer>
#include <QObject>

#include "types.h"

class QTcpSocket;

namespace Http
{
    class IRequestHandler;

    class Connection : public QObject
    {
        Q_OBJECT
        Q_DISABLE_COPY(Connection)

    public:
        Connection(QTcpSocket *socket, IRequestHandler *requestHandler, QObject *parent = 0);
        ~Connection();

        bool hasExpired(qint64 timeout) const;
        bool isClosed() const;

    private slots:
        void read();

    private:
        static bool acceptsGzipEncoding(QString codings);
        void sendResponse(const Response &response) const;

        QTcpSocket *m_socket;
        IRequestHandler *m_requestHandler;
        QByteArray m_receivedData;
        QElapsedTimer m_idleTimer;
    };
}

#endif // HTTP_CONNECTION_H
