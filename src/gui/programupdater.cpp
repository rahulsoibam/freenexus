#include <QXmlStreamReader>
#include <QDesktopServices>
#include <QDebug>
#include <QRegExp>
#include <QStringList>

#include "base/utils/fs.h"
#include "base/net/downloadmanager.h"
#include "base/net/downloadhandler.h"
#include "programupdater.h"

namespace
{
    const QString RSS_URL("https://github.com/rahulsoibam/freenexus");

#ifdef Q_OS_MAC
    const QString OS_TYPE("Mac OS X");
#elif defined(Q_OS_WIN) && (defined(__x86_64__) || defined(_M_X64))
    const QString OS_TYPE("Windows x64");
#else
    const QString OS_TYPE("Windows");
#endif

    QString getStringValue(QXmlStreamReader &xml);
}

ProgramUpdater::ProgramUpdater(QObject *parent, bool invokedByUser)
    : QObject(parent)
    , m_invokedByUser(invokedByUser)
{
}

void ProgramUpdater::checkForUpdates()
{
    Net::DownloadHandler *handler = Net::DownloadManager::instance()->downloadUrl(
                RSS_URL, false, 0, false,
                // Don't change this User-Agent. In case our updater goes haywire,
                // the filehost can identify it and contact us.
                "freeNexus/" QBT_VERSION_2 " ProgramUpdater (www.freenexus.org)");
    connect(handler, SIGNAL(downloadFinished(QString,QByteArray)), SLOT(rssDownloadFinished(QString,QByteArray)));
    connect(handler, SIGNAL(downloadFailed(QString,QString)), SLOT(rssDownloadFailed(QString,QString)));
}

void ProgramUpdater::rssDownloadFinished(const QString &url, const QByteArray &data)
{
    Q_UNUSED(url);

    qDebug("Finished downloading the new freeNexus updates RSS");
    QString version;

    QXmlStreamReader xml(data);
    bool inItem = false;
    QString updateLink;
    QString type;

    while (!xml.atEnd()) {
        xml.readNext();

        if (xml.isStartElement()) {
            if (xml.name() == "item")
                inItem = true;
            else if (inItem && xml.name() == "link")
                updateLink = getStringValue(xml);
            else if (inItem && xml.name() == "type")
                type = getStringValue(xml);
            else if (inItem && xml.name() == "version")
                version = getStringValue(xml);
        }
        else if (xml.isEndElement()) {
            if (inItem && xml.name() == "item") {
                if (type.compare(OS_TYPE, Qt::CaseInsensitive) == 0) {
                    qDebug("The last update available is %s", qUtf8Printable(version));
                    if (!version.isEmpty()) {
                        qDebug("Detected version is %s", qUtf8Printable(version));
                        if (isVersionMoreRecent(version))
                            m_updateUrl = updateLink;
                    }
                    break;
                }

                inItem = false;
                updateLink.clear();
                type.clear();
                version.clear();
            }
        }
    }

    emit updateCheckFinished(!m_updateUrl.isEmpty(), version, m_invokedByUser);
}

void ProgramUpdater::rssDownloadFailed(const QString &url, const QString &error)
{
    Q_UNUSED(url);

    qDebug() << "Downloading the new freeNexus updates RSS failed:" << error;
    emit updateCheckFinished(false, QString(), m_invokedByUser);
}

void ProgramUpdater::updateProgram()
{
    Q_ASSERT(!m_updateUrl.isEmpty());
    QDesktopServices::openUrl(m_updateUrl);
    return;
}

bool ProgramUpdater::isVersionMoreRecent(const QString &remoteVersion) const
{
    QRegExp regVer("([0-9.]+)");
    if (regVer.indexIn(QBT_VERSION) >= 0) {
        QString localVersion = regVer.cap(1);
        qDebug() << Q_FUNC_INFO << "local version:" << localVersion << "/" << QBT_VERSION;
        QStringList remoteParts = remoteVersion.split('.');
        QStringList localParts = localVersion.split('.');
        for (int i = 0; i<qMin(remoteParts.size(), localParts.size()); ++i) {
            if (remoteParts[i].toInt() > localParts[i].toInt())
                return true;
            if (remoteParts[i].toInt() < localParts[i].toInt())
                return false;
        }
        // Compared parts were equal, if remote version is longer, then it's more recent (2.9.2.1 > 2.9.2)
        if (remoteParts.size() > localParts.size())
            return true;
        // versions are equal, check if the local version is a development release, in which case it is older (2.9.2beta < 2.9.2)
        QRegExp regDevel("(alpha|beta|rc)");
        if (regDevel.indexIn(QBT_VERSION) >= 0)
            return true;
    }
    return false;
}

namespace
{
    QString getStringValue(QXmlStreamReader &xml)
    {
        xml.readNext();
        if (xml.isCharacters() && !xml.isWhitespace())
            return xml.text().toString();

        return QString();
    }
}
