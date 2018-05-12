#ifndef DNSUPDATER_H
#define DNSUPDATER_H

#include <QDateTime>
#include <QHostAddress>
#include <QObject>
#include <QTimer>

#include "base/preferences.h"

namespace Net
{
    // Based on http://www.dyndns.com/developers/specs/
    class DNSUpdater : public QObject
    {
        Q_OBJECT

    public:
        explicit DNSUpdater(QObject *parent = nullptr);
        ~DNSUpdater();

        static QUrl getRegistrationUrl(int service);

    public slots:
        void updateCredentials();

    private slots:
        void checkPublicIP();
        void ipRequestFinished(const QString &url, const QByteArray &data);
        void ipRequestFailed(const QString &url, const QString &error);
        void updateDNSService();
        void ipUpdateFinished(const QString &url, const QByteArray &data);
        void ipUpdateFailed(const QString &url, const QString &error);

    private:
        enum State
        {
            OK,
            INVALID_CREDS,
            FATAL
        };

        static const int IP_CHECK_INTERVAL_MS = 1800000; // 30 min

        QString getUpdateUrl() const;
        void processIPUpdateReply(const QString &reply);

        QHostAddress m_lastIP;
        QDateTime m_lastIPCheckTime;
        QTimer m_ipCheckTimer;
        int m_state;
        // Service creds
        DNS::Service m_service;
        QString m_domain;
        QString m_username;
        QString m_password;
    };
}

#endif // DNSUPDATER_H
