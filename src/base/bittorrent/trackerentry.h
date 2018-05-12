#ifndef BITTORRENT_TRACKERENTRY_H
#define BITTORRENT_TRACKERENTRY_H

#include <libtorrent/torrent_info.hpp>
#include <libtorrent/version.hpp>
#if LIBTORRENT_VERSION_NUM >= 10100
#include <libtorrent/announce_entry.hpp>
#endif

class QString;

namespace BitTorrent
{
    class TrackerEntry
    {
    public:
        enum Status
        {
            NotContacted,
            Working,
            Updating,
            NotWorking
        };

        TrackerEntry(const QString &url);
        TrackerEntry(const libtorrent::announce_entry &nativeEntry);
        TrackerEntry(const TrackerEntry &other);

        QString url() const;
        int tier() const;
        Status status() const;

        void setTier(int value);
        TrackerEntry &operator=(const TrackerEntry &other);
        bool operator==(const TrackerEntry &other) const;

        libtorrent::announce_entry nativeEntry() const;

    private:
        libtorrent::announce_entry m_nativeEntry;
    };
}

#endif // BITTORRENT_TRACKERENTRY_H
