#ifndef NET_PROXYCONFIGURATIONMANAGER_H
#define NET_PROXYCONFIGURATIONMANAGER_H

#include <QObject>

namespace Net
{
    enum class ProxyType
    {
        None = 0,
        HTTP = 1,
        SOCKS5 = 2,
        HTTP_PW = 3,
        SOCKS5_PW = 4,
        SOCKS4 = 5
    };

    struct ProxyConfiguration
    {
        ProxyType type = ProxyType::None;
        QString ip = "0.0.0.0";
        ushort port = 8080;
        QString username;
        QString password;
    };

    class ProxyConfigurationManager: public QObject
    {
        Q_OBJECT
        Q_DISABLE_COPY(ProxyConfigurationManager)

        explicit ProxyConfigurationManager(QObject *parent = nullptr);
        ~ProxyConfigurationManager() = default;

    public:
        static void initInstance();
        static void freeInstance();
        static ProxyConfigurationManager *instance();

        ProxyConfiguration proxyConfiguration() const;
        void setProxyConfiguration(const ProxyConfiguration &config);
        bool isProxyOnlyForTorrents() const;
        void setProxyOnlyForTorrents(bool onlyForTorrents);

        bool isAuthenticationRequired() const;

    signals:
        void proxyConfigurationChanged();

    private:
        void configureProxy();

        static ProxyConfigurationManager *m_instance;
        ProxyConfiguration m_config;
        bool m_isProxyOnlyForTorrents;
    };
}

#endif // NET_PROXYCONFIGURATIONMANAGER_H
