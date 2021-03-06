/*
 * This code is based on QxtSmtp from libqxt (http://libqxt.org)
 */

#ifndef SMTP_H
#define SMTP_H

#include <QAbstractSocket>
#include <QByteArray>
#include <QHash>
#include <QMetaType>
#include <QObject>
#include <QString>

#ifndef QT_NO_OPENSSL
class QSslSocket;
#else
class QTcpSocket;
#endif
class QTextCodec;
class QTextStream;

namespace Net
{
    class Smtp : public QObject
    {
        Q_OBJECT

    public:
        Smtp(QObject *parent = nullptr);
        ~Smtp();

        void sendMail(const QString &from, const QString &to, const QString &subject, const QString &body);

    private slots:
        void readyRead();
        void error(QAbstractSocket::SocketError socketError);

    private:
        enum States
        {
            Rcpt,
            EhloSent,
            HeloSent,
            EhloDone,
            EhloGreetReceived,
            AuthRequestSent,
            AuthSent,
            AuthUsernameSent,
            Authenticated,
            StartTLSSent,
            Data,
            Init,
            Body,
            Quit,
            Close
        };

        enum AuthType
        {
            AuthPlain,
            AuthLogin,
            AuthCramMD5
        };

        QByteArray encodeMimeHeader(const QString &key, const QString &value, QTextCodec *latin1, const QByteArray &prefix = QByteArray());
        void ehlo();
        void helo();
        void parseEhloResponse(const QByteArray &code, bool continued, const QString &line);
        void authenticate();
        void startTLS();
        void authCramMD5(const QByteArray &challenge = QByteArray());
        void authPlain();
        void authLogin();
        void logError(const QString &msg);
        QString getCurrentDateTime() const;

        QByteArray m_message;
#ifndef QT_NO_OPENSSL
        QSslSocket *m_socket;
#else
        QTcpSocket *m_socket;
#endif
        QString m_from;
        QString m_rcpt;
        QString m_response;
        int m_state;
        QHash<QString, QString> m_extensions;
        QByteArray m_buffer;
        bool m_useSsl;
        AuthType m_authType;
        QString m_username;
        QString m_password;
    };
}

#endif // SMTP_H
