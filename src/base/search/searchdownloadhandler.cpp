#include "searchdownloadhandler.h"

#include <QProcess>

#include "../utils/fs.h"
#include "../utils/misc.h"
#include "searchpluginmanager.h"

SearchDownloadHandler::SearchDownloadHandler(const QString &siteUrl, const QString &url, SearchPluginManager *manager)
    : QObject {manager}
    , m_manager {manager}
    , m_downloadProcess {new QProcess {this}}
{
    m_downloadProcess->setEnvironment(QProcess::systemEnvironment());
    connect(m_downloadProcess, static_cast<void (QProcess::*)(int)>(&QProcess::finished)
            , this, &SearchDownloadHandler::downloadProcessFinished);
    const QStringList params {
        Utils::Fs::toNativePath(m_manager->engineLocation() + "/nova2dl.py"),
        siteUrl,
        url
    };
    // Launch search
    m_downloadProcess->start(Utils::Misc::pythonExecutable(), params, QIODevice::ReadOnly);
}

void SearchDownloadHandler::downloadProcessFinished(int exitcode)
{
    QString path;

    if ((exitcode == 0) && (m_downloadProcess->exitStatus() == QProcess::NormalExit)) {
        const QString line = QString::fromUtf8(m_downloadProcess->readAllStandardOutput()).trimmed();
        const QVector<QStringRef> parts = line.splitRef(' ');
        if (parts.size() == 2)
            path = parts[0].toString();
    }

    emit downloadFinished(path);
}
