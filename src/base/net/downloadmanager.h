#ifndef NET_DOWNLOADMANAGER_H
#define NET_DOWNLOADMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>

class QNetworkReply;
class QNetworkCookie;
class QSslError;
class QUrl;

namespace Net
{
    class DownloadHandler;

    class DownloadManager : public QObject
    {
        Q_OBJECT

    public:
        static void initInstance();
        static void freeInstance();
        static DownloadManager *instance();

        DownloadHandler *downloadUrl(const QString &url, bool saveToFile = false, qint64 limit = 0, bool handleRedirectToMagnet = false, const QString &userAgent = "");
        QList<QNetworkCookie> cookiesForUrl(const QUrl &url) const;
        bool setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url);
        QList<QNetworkCookie> allCookies() const;
        void setAllCookies(const QList<QNetworkCookie> &cookieList);
        bool deleteCookie(const QNetworkCookie &cookie);

    private slots:
    #ifndef QT_NO_OPENSSL
        void ignoreSslErrors(QNetworkReply *,const QList<QSslError> &);
    #endif

    private:
        explicit DownloadManager(QObject *parent = 0);

        void applyProxySettings();

        static DownloadManager *m_instance;
        QNetworkAccessManager m_networkManager;
    };
}

#endif // NET_DOWNLOADMANAGER_H
