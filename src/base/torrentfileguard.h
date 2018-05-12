#ifndef TOFFENTFILEGURAD_H
#define TOFFENTFILEGURAD_H

#include <QObject>
#include <QString>

template <typename T> class CachedSettingValue;

/// Utility class to defer file deletion
class FileGuard
{
public:
    FileGuard(const QString &path = QString());
    ~FileGuard();

    /// Cancels or re-enables deferred file deletion
    void setAutoRemove(bool remove) noexcept;

private:
    QString m_path;
    bool m_remove;
};

/// Reads settings for .torrent files from preferences
/// and sets the file guard up accordingly
class TorrentFileGuard: private FileGuard
{
    Q_GADGET

public:
    TorrentFileGuard(const QString &path = QString());
    ~TorrentFileGuard();

    /// marks the torrent file as loaded (added) into the BitTorrent::Session
    void markAsAddedToSession();
    using FileGuard::setAutoRemove;

    enum AutoDeleteMode: int     // do not change these names: they are stored in config file
    {
        Never,
        IfAdded,
        Always
    };

    // static interface to get/set preferences
    static AutoDeleteMode autoDeleteMode();
    static void setAutoDeleteMode(AutoDeleteMode mode);

private:
    TorrentFileGuard(const QString &path, AutoDeleteMode mode);
    static CachedSettingValue<AutoDeleteMode> &autoDeleteModeSetting();

    Q_ENUM(AutoDeleteMode)
    AutoDeleteMode m_mode;
    bool m_wasAdded;
};

#endif // TOFFENTFILEGURAD_H
