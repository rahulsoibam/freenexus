#include "bittorrent/torrenthandle.h"
#include "torrentfilter.h"

const QString TorrentFilter::AnyCategory;
const QStringSet TorrentFilter::AnyHash = (QStringSet() << QString());
const QString TorrentFilter::AnyTag;

const TorrentFilter TorrentFilter::DownloadingTorrent(TorrentFilter::Downloading);
const TorrentFilter TorrentFilter::SeedingTorrent(TorrentFilter::Seeding);
const TorrentFilter TorrentFilter::CompletedTorrent(TorrentFilter::Completed);
const TorrentFilter TorrentFilter::PausedTorrent(TorrentFilter::Paused);
const TorrentFilter TorrentFilter::ResumedTorrent(TorrentFilter::Resumed);
const TorrentFilter TorrentFilter::ActiveTorrent(TorrentFilter::Active);
const TorrentFilter TorrentFilter::InactiveTorrent(TorrentFilter::Inactive);
const TorrentFilter TorrentFilter::ErroredTorrent(TorrentFilter::Errored);

using BitTorrent::TorrentHandle;
using BitTorrent::TorrentState;

TorrentFilter::TorrentFilter()
    : m_type(All)
{
}

TorrentFilter::TorrentFilter(const Type type, const QStringSet &hashSet, const QString &category, const QString &tag)
    : m_type(type)
    , m_category(category)
    , m_tag(tag)
    , m_hashSet(hashSet)
{
}

TorrentFilter::TorrentFilter(const QString &filter, const QStringSet &hashSet, const QString &category, const QString &tag)
    : m_type(All)
    , m_category(category)
    , m_tag(tag)
    , m_hashSet(hashSet)
{
    setTypeByName(filter);
}

bool TorrentFilter::setType(Type type)
{
    if (m_type != type) {
        m_type = type;
        return true;
    }

    return false;
}

bool TorrentFilter::setTypeByName(const QString &filter)
{
    Type type = All;

    if (filter == "downloading")
        type = Downloading;
    else if (filter == "seeding")
        type = Seeding;
    else if (filter == "completed")
        type = Completed;
    else if (filter == "paused")
        type = Paused;
    else if (filter == "resumed")
        type = Resumed;
    else if (filter == "active")
        type = Active;
    else if (filter == "inactive")
        type = Inactive;
    else if (filter == "errored")
        type = Errored;

    return setType(type);
}

bool TorrentFilter::setHashSet(const QStringSet &hashSet)
{
    if (m_hashSet != hashSet) {
        m_hashSet = hashSet;
        return true;
    }

    return false;
}

bool TorrentFilter::setCategory(const QString &category)
{
    // QString::operator==() doesn't distinguish between empty and null strings.
    if ((m_category != category)
            || (m_category.isNull() && !category.isNull())
            || (!m_category.isNull() && category.isNull())) {
        m_category = category;
        return true;
    }

    return false;
}

bool TorrentFilter::setTag(const QString &tag)
{
    // QString::operator==() doesn't distinguish between empty and null strings.
    if ((m_tag != tag)
        || (m_tag.isNull() && !tag.isNull())
        || (!m_tag.isNull() && tag.isNull())) {
        m_tag = tag;
        return true;
    }

    return false;
}

bool TorrentFilter::match(TorrentHandle *const torrent) const
{
    if (!torrent) return false;

    return (matchState(torrent) && matchHash(torrent) && matchCategory(torrent) && matchTag(torrent));
}

bool TorrentFilter::matchState(BitTorrent::TorrentHandle *const torrent) const
{
    switch (m_type) {
    case All:
        return true;
    case Downloading:
        return torrent->isDownloading();
    case Seeding:
        return torrent->isUploading();
    case Completed:
        return torrent->isCompleted();
    case Paused:
        return torrent->isPaused();
    case Resumed:
        return torrent->isResumed();
    case Active:
        return torrent->isActive();
    case Inactive:
        return torrent->isInactive();
    case Errored:
        return torrent->isErrored();
    default: // All
        return true;
    }
}

bool TorrentFilter::matchHash(BitTorrent::TorrentHandle *const torrent) const
{
    if (m_hashSet == AnyHash) return true;
    else return m_hashSet.contains(torrent->hash());
}

bool TorrentFilter::matchCategory(BitTorrent::TorrentHandle *const torrent) const
{
    if (m_category.isNull()) return true;
    else return (torrent->belongsToCategory(m_category));
}

bool TorrentFilter::matchTag(BitTorrent::TorrentHandle *const torrent) const
{
    // Empty tag is a special value to indicate we're filtering for untagged torrents.
    if (m_tag.isNull()) return true;
    else if (m_tag.isEmpty()) return torrent->tags().isEmpty();
    else return (torrent->hasTag(m_tag));
}
