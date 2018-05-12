#include "filesystemwatcher.h"

#if defined(Q_OS_MAC) || defined(Q_OS_FREEBSD)
#include <cstring>
#include <sys/mount.h>
#include <sys/param.h>
#endif

#include <QtGlobal>

#include "base/algorithm.h"
#include "base/bittorrent/magneturi.h"
#include "base/bittorrent/torrentinfo.h"
#include "base/global.h"
#include "base/preferences.h"
#include "base/utils/fs.h"

namespace
{
    const int WATCH_INTERVAL = 10000; // 10 sec
    const int MAX_PARTIAL_RETRIES = 5;
}

FileSystemWatcher::FileSystemWatcher(QObject *parent)
    : QFileSystemWatcher(parent)
{
    connect(this, &QFileSystemWatcher::directoryChanged, this, &FileSystemWatcher::scanLocalFolder);

    m_partialTorrentTimer.setSingleShot(true);
    connect(&m_partialTorrentTimer, &QTimer::timeout, this, &FileSystemWatcher::processPartialTorrents);

#ifndef Q_OS_WIN
    connect(&m_watchTimer, &QTimer::timeout, this, &FileSystemWatcher::scanNetworkFolders);
#endif
}

QStringList FileSystemWatcher::directories() const
{
    QStringList dirs = QFileSystemWatcher::directories();
#ifndef Q_OS_WIN
    for (const QDir &dir : qAsConst(m_watchedFolders))
        dirs << dir.canonicalPath();
#endif
    return dirs;
}

void FileSystemWatcher::addPath(const QString &path)
{
    if (path.isEmpty()) return;

#if !defined Q_OS_WIN && !defined Q_OS_HAIKU
    const QDir dir(path);
    if (!dir.exists()) return;

    // Check if the path points to a network file system or not
    if (Utils::Fs::isNetworkFileSystem(path)) {
        // Network mode
        qDebug("Network folder detected: %s", qUtf8Printable(path));
        qDebug("Using file polling mode instead of inotify...");
        m_watchedFolders << dir;

        m_watchTimer.start(WATCH_INTERVAL);
        return;
    }
#endif

    // Normal mode
    qDebug("FS Watcher is watching %s in normal mode", qUtf8Printable(path));
    QFileSystemWatcher::addPath(path);
    scanLocalFolder(path);
}

void FileSystemWatcher::removePath(const QString &path)
{
#ifndef Q_OS_WIN
    if (m_watchedFolders.removeOne(path)) {
        if (m_watchedFolders.isEmpty())
            m_watchTimer.stop();
        return;
    }
#endif
    // Normal mode
    QFileSystemWatcher::removePath(path);
}

void FileSystemWatcher::scanLocalFolder(const QString &path)
{
    QTimer::singleShot(2000, this, [this, path]() { processTorrentsInDir(path); });
}

#ifndef Q_OS_WIN
void FileSystemWatcher::scanNetworkFolders()
{
    for (const QDir &dir : qAsConst(m_watchedFolders))
        processTorrentsInDir(dir);
}
#endif

void FileSystemWatcher::processPartialTorrents()
{
    QStringList noLongerPartial;

    // Check which torrents are still partial
    Dict::removeIf(m_partialTorrents, [&noLongerPartial](const QString &torrentPath, int &value)
    {
        if (!QFile::exists(torrentPath))
            return true;

        if (BitTorrent::TorrentInfo::loadFromFile(torrentPath).isValid()) {
            noLongerPartial << torrentPath;
            return true;
        }

        if (value >= MAX_PARTIAL_RETRIES) {
            QFile::rename(torrentPath, torrentPath + ".qbt_rejected");
            return true;
        }

        ++value;
        return false;
    });

    // Stop the partial timer if necessary
    if (m_partialTorrents.empty()) {
        m_partialTorrentTimer.stop();
        qDebug("No longer any partial torrent.");
    }
    else {
        qDebug("Still %d partial torrents after delayed processing.", m_partialTorrents.count());
        m_partialTorrentTimer.start(WATCH_INTERVAL);
    }

    // Notify of new torrents
    if (!noLongerPartial.isEmpty())
        emit torrentsAdded(noLongerPartial);
}

void FileSystemWatcher::processTorrentsInDir(const QDir &dir)
{
    QStringList torrents;
    const QStringList files = dir.entryList({"*.torrent", "*.magnet"}, QDir::Files);
    for (const QString &file : files) {
        const QString fileAbsPath = dir.absoluteFilePath(file);
        if (file.endsWith(".magnet"))
            torrents << fileAbsPath;
        else if (BitTorrent::TorrentInfo::loadFromFile(fileAbsPath).isValid())
            torrents << fileAbsPath;
        else if (!m_partialTorrents.contains(fileAbsPath))
            m_partialTorrents[fileAbsPath] = 0;
    }

    if (!torrents.empty())
        emit torrentsAdded(torrents);

    if (!m_partialTorrents.empty() && !m_partialTorrentTimer.isActive())
        m_partialTorrentTimer.start(WATCH_INTERVAL);
}
