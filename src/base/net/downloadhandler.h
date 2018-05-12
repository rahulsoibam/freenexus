#ifndef NET_DOWNLOADHANDLER_H
#define NET_DOWNLOADHANDLER_H

#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;
class QUrl;

namespace Net
{
    class DownloadManager;

    class DownloadHandler : public QObject
    {
        Q_OBJECT

    public:
        DownloadHandler(QNetworkReply *reply, DownloadManager *manager, bool saveToFile = false, qint64 limit = 0, bool handleRedirectToMagnet = false);
        ~DownloadHandler();

        QString url() const;

    signals:
        void downloadFinished(const QString &url, const QByteArray &data);
        void downloadFinished(const QString &url, const QString &filePath);
        void downloadFailed(const QString &url, const QString &reason);
        void redirectedToMagnet(const QString &url, const QString &magnetUri);

    private slots:
        void processFinishedDownload();
        void checkDownloadSize(qint64 bytesReceived, qint64 bytesTotal);

    private:
        void init();
        bool saveToFile(const QByteArray &replyData, QString &filePath);
        void handleRedirection(QUrl newUrl);

        QNetworkReply *m_reply;
        DownloadManager *m_manager;
        bool m_saveToFile;
        qint64 m_sizeLimit;
        bool m_handleRedirectToMagnet;
        QString m_url;
    };
}

#endif // NET_DOWNLOADHANDLER_H
