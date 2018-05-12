#include "torrentfileguard.h"

#include "settingvalue.h"
#include "utils/fs.h"

FileGuard::FileGuard(const QString &path)
    : m_path {path}
    , m_remove {true}
{
}

void FileGuard::setAutoRemove(bool remove) noexcept
{
    m_remove = remove;
}

FileGuard::~FileGuard()
{
    if (m_remove && !m_path.isEmpty())
        Utils::Fs::forceRemove(m_path); // forceRemove() checks for file existence
}

TorrentFileGuard::TorrentFileGuard(const QString &path, TorrentFileGuard::AutoDeleteMode mode)
    : FileGuard {mode != Never ? path : QString()}
    , m_mode {mode}
    , m_wasAdded {false}
{
}

TorrentFileGuard::TorrentFileGuard(const QString &path)
    : TorrentFileGuard {path, autoDeleteMode()}
{
}

TorrentFileGuard::~TorrentFileGuard()
{
    if (!m_wasAdded && (m_mode != Always))
        setAutoRemove(false);
}

void TorrentFileGuard::markAsAddedToSession()
{
    m_wasAdded = true;
}

TorrentFileGuard::AutoDeleteMode TorrentFileGuard::autoDeleteMode()
{
    return autoDeleteModeSetting();
}

void TorrentFileGuard::setAutoDeleteMode(TorrentFileGuard::AutoDeleteMode mode)
{
    autoDeleteModeSetting() = mode;
}

CachedSettingValue<TorrentFileGuard::AutoDeleteMode> &TorrentFileGuard::autoDeleteModeSetting()
{
    static CachedSettingValue<AutoDeleteMode> setting("Core/AutoDeleteAddedTorrentFile", AutoDeleteMode::Never);
    return setting;
}

