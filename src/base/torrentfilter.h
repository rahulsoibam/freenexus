#ifndef TORRENTFILTER_H
#define TORRENTFILTER_H

#include <QString>
#include <QSet>

typedef QSet<QString> QStringSet;

namespace BitTorrent
{
    class TorrentHandle;
}

class TorrentFilter
{
public:
    enum Type
    {
        All,
        Downloading,
        Seeding,
        Completed,
        Resumed,
        Paused,
        Active,
        Inactive,
        Errored
    };

    // These mean any permutation, including no category / tag.
    static const QString AnyCategory;
    static const QStringSet AnyHash;
    static const QString AnyTag; 

    static const TorrentFilter DownloadingTorrent;
    static const TorrentFilter SeedingTorrent;
    static const TorrentFilter CompletedTorrent;
    static const TorrentFilter PausedTorrent;
    static const TorrentFilter ResumedTorrent;
    static const TorrentFilter ActiveTorrent;
    static const TorrentFilter InactiveTorrent;
    static const TorrentFilter ErroredTorrent;

    TorrentFilter();
    // category & tags: pass empty string for uncategorized / untagged torrents.
    // Pass null string (QString()) to disable filtering (i.e. all torrents).
    TorrentFilter(const Type type, const QStringSet &hashSet = AnyHash, const QString &category = AnyCategory, const QString &tag = AnyTag);
    TorrentFilter(const QString &filter, const QStringSet &hashSet = AnyHash, const QString &category = AnyCategory, const QString &tags = AnyTag);

    bool setType(Type type);
    bool setTypeByName(const QString &filter);
    bool setHashSet(const QStringSet &hashSet);
    bool setCategory(const QString &category);
    bool setTag(const QString &tag);

    bool match(BitTorrent::TorrentHandle *const torrent) const;

private:
    bool matchState(BitTorrent::TorrentHandle *const torrent) const;
    bool matchHash(BitTorrent::TorrentHandle *const torrent) const;
    bool matchCategory(BitTorrent::TorrentHandle *const torrent) const;
    bool matchTag(BitTorrent::TorrentHandle *const torrent) const;

    Type m_type;
    QString m_category;
    QString m_tag;
    QStringSet m_hashSet;
};

#endif // TORRENTFILTER_H
