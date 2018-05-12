#ifndef FILESYSTEMWATCHER_H
#define FILESYSTEMWATCHER_H

#include <QDir>
#include <QFileSystemWatcher>
#include <QHash>
#include <QStringList>
#include <QTimer>

/*
 * Subclassing QFileSystemWatcher in order to support Network File
 * System watching (NFS, CIFS) on Linux and Mac OS.
 */
class FileSystemWatcher : public QFileSystemWatcher
{
    Q_OBJECT

public:
    explicit FileSystemWatcher(QObject *parent = nullptr);

    QStringList directories() const;
    void addPath(const QString &path);
    void removePath(const QString &path);

signals:
    void torrentsAdded(const QStringList &pathList);

protected slots:
    void scanLocalFolder(const QString &path);
    void processPartialTorrents();
#ifndef Q_OS_WIN
    void scanNetworkFolders();
#endif

private:
    void processTorrentsInDir(const QDir &dir);

    // Partial torrents
    QHash<QString, int> m_partialTorrents;
    QTimer m_partialTorrentTimer;

#ifndef Q_OS_WIN
    QList<QDir> m_watchedFolders;
    QTimer m_watchTimer;
#endif
};

#endif // FILESYSTEMWATCHER_H
