#include "downloadmanager.h"

#include <QDateTime>
#include <QDebug>
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QNetworkProxy>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSslError>
#include <QUrl>

#include "base/preferences.h"
#include "downloadhandler.h"
#include "proxyconfigurationmanager.h"

// Spoof Firefox 38 user agent to avoid web server banning
const char DEFAULT_USER_AGENT[] = "Mozilla/5.0 (X11; Linux i686; rv:38.0) Gecko/20100101 Firefox/38.0";

namespace
{
    class NetworkCookieJar: public QNetworkCookieJar
    {
    public:
        explicit NetworkCookieJar(QObject *parent = 0)
            : QNetworkCookieJar(parent)
        {
            QDateTime now = QDateTime::currentDateTime();
            QList<QNetworkCookie> cookies = Preferences::instance()->getNetworkCookies();
            foreach (const QNetworkCookie &cookie, Preferences::instance()->getNetworkCookies()) {
                if (cookie.isSessionCookie() || (cookie.expirationDate() <= now))
                    cookies.removeAll(cookie);
            }

            setAllCookies(cookies);
        }

        ~NetworkCookieJar()
        {
            QDateTime now = QDateTime::currentDateTime();
            QList<QNetworkCookie> cookies = allCookies();
            foreach (const QNetworkCookie &cookie, allCookies()) {
                if (cookie.isSessionCookie() || (cookie.expirationDate() <= now))
                    cookies.removeAll(cookie);
            }

            Preferences::instance()->setNetworkCookies(cookies);
        }

        using QNetworkCookieJar::allCookies;
        using QNetworkCookieJar::setAllCookies;

        QList<QNetworkCookie> cookiesForUrl(const QUrl &url) const override
        {
            QDateTime now = QDateTime::currentDateTime();
            QList<QNetworkCookie> cookies = QNetworkCookieJar::cookiesForUrl(url);
            foreach (const QNetworkCookie &cookie, QNetworkCookieJar::cookiesForUrl(url)) {
                if (!cookie.isSessionCookie() && (cookie.expirationDate() <= now))
                    cookies.removeAll(cookie);
            }

            return cookies;
        }

        bool setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url) override
        {
            QDateTime now = QDateTime::currentDateTime();
            QList<QNetworkCookie> cookies = cookieList;
            foreach (const QNetworkCookie &cookie, cookieList) {
                if (!cookie.isSessionCookie() && (cookie.expirationDate() <= now))
                    cookies.removeAll(cookie);
            }

            return QNetworkCookieJar::setCookiesFromUrl(cookies, url);
        }
    };
}

using namespace Net;

DownloadManager *DownloadManager::m_instance = 0;

DownloadManager::DownloadManager(QObject *parent)
    : QObject(parent)
{
#ifndef QT_NO_OPENSSL
    connect(&m_networkManager, SIGNAL(sslErrors(QNetworkReply *, QList<QSslError>)), this, SLOT(ignoreSslErrors(QNetworkReply *, QList<QSslError>)));
#endif
    m_networkManager.setCookieJar(new NetworkCookieJar(this));
}

void DownloadManager::initInstance()
{
    if (!m_instance)
        m_instance = new DownloadManager;
}

void DownloadManager::freeInstance()
{
    if (m_instance) {
        delete m_instance;
        m_instance = 0;
    }
}

DownloadManager *DownloadManager::instance()
{
    return m_instance;
}

DownloadHandler *DownloadManager::downloadUrl(const QString &url, bool saveToFile, qint64 limit, bool handleRedirectToMagnet, const QString &userAgent)
{
    // Update proxy settings
    applyProxySettings();

    // Process download request
    qDebug("url is %s", qUtf8Printable(url));
    const QUrl qurl = QUrl(url);
    QNetworkRequest request(qurl);

    if (userAgent.isEmpty())
        request.setRawHeader("User-Agent", DEFAULT_USER_AGENT);
    else
        request.setRawHeader("User-Agent", userAgent.toUtf8());

    // Spoof HTTP Referer to allow adding torrent link from Torcache/KickAssTorrents
    request.setRawHeader("Referer", request.url().toEncoded().data());

    qDebug("Downloading %s...", request.url().toEncoded().data());
    qDebug() << "Cookies:" << m_networkManager.cookieJar()->cookiesForUrl(request.url());
    // accept gzip
    request.setRawHeader("Accept-Encoding", "gzip");
    return new DownloadHandler(m_networkManager.get(request), this, saveToFile, limit, handleRedirectToMagnet);
}

QList<QNetworkCookie> DownloadManager::cookiesForUrl(const QUrl &url) const
{
    return m_networkManager.cookieJar()->cookiesForUrl(url);
}

bool DownloadManager::setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url)
{
    return m_networkManager.cookieJar()->setCookiesFromUrl(cookieList, url);
}

QList<QNetworkCookie> DownloadManager::allCookies() const
{
    return static_cast<NetworkCookieJar *>(m_networkManager.cookieJar())->allCookies();
}

void DownloadManager::setAllCookies(const QList<QNetworkCookie> &cookieList)
{
    static_cast<NetworkCookieJar *>(m_networkManager.cookieJar())->setAllCookies(cookieList);
}

bool DownloadManager::deleteCookie(const QNetworkCookie &cookie)
{
    return static_cast<NetworkCookieJar *>(m_networkManager.cookieJar())->deleteCookie(cookie);
}

void DownloadManager::applyProxySettings()
{
    auto proxyManager = ProxyConfigurationManager::instance();
    ProxyConfiguration proxyConfig = proxyManager->proxyConfiguration();
    QNetworkProxy proxy;

    if (!proxyManager->isProxyOnlyForTorrents() && (proxyConfig.type != ProxyType::None)) {
        // Proxy enabled
        proxy.setHostName(proxyConfig.ip);
        proxy.setPort(proxyConfig.port);
        // Default proxy type is HTTP, we must change if it is SOCKS5
        if ((proxyConfig.type == ProxyType::SOCKS5) || (proxyConfig.type == ProxyType::SOCKS5_PW)) {
            qDebug() << Q_FUNC_INFO << "using SOCKS proxy";
            proxy.setType(QNetworkProxy::Socks5Proxy);
        }
        else {
            qDebug() << Q_FUNC_INFO << "using HTTP proxy";
            proxy.setType(QNetworkProxy::HttpProxy);
        }
        // Authentication?
        if (proxyManager->isAuthenticationRequired()) {
            qDebug("Proxy requires authentication, authenticating");
            proxy.setUser(proxyConfig.username);
            proxy.setPassword(proxyConfig.password);
        }
    }
    else {
        proxy.setType(QNetworkProxy::NoProxy);
    }

    m_networkManager.setProxy(proxy);
}

#ifndef QT_NO_OPENSSL
void DownloadManager::ignoreSslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
    Q_UNUSED(errors)
    // Ignore all SSL errors
    reply->ignoreSslErrors();
}
#endif
