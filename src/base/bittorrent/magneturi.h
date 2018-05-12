#ifndef BITTORRENT_MAGNETURI_H
#define BITTORRENT_MAGNETURI_H

#include <QList>
#include <QString>
#include <QUrl>

#include <libtorrent/add_torrent_params.hpp>

#include "infohash.h"
#include "trackerentry.h"

namespace BitTorrent
{
    class MagnetUri
    {
    public:
        explicit MagnetUri(const QString &source = QString());

        bool isValid() const;
        InfoHash hash() const;
        QString name() const;
        QList<TrackerEntry> trackers() const;
        QList<QUrl> urlSeeds() const;
        QString url() const;

        libtorrent::add_torrent_params addTorrentParams() const;

    private:
        bool m_valid;
        QString m_url;
        InfoHash m_hash;
        QString m_name;
        QList<TrackerEntry> m_trackers;
        QList<QUrl> m_urlSeeds;
        libtorrent::add_torrent_params m_addTorrentParams;
    };
}

#endif // BITTORRENT_MAGNETURI_H
