#ifndef BITTORRENT_INFOHASH_H
#define BITTORRENT_INFOHASH_H

#include <QString>
#include <libtorrent/sha1_hash.hpp>

namespace BitTorrent
{
    class InfoHash
    {
    public:
        InfoHash();
        InfoHash(const libtorrent::sha1_hash &nativeHash);
        InfoHash(const QString &hashString);
        InfoHash(const InfoHash &other);

        bool isValid() const;

        operator libtorrent::sha1_hash() const;
        operator QString() const;
        bool operator==(const InfoHash &other) const;
        bool operator!=(const InfoHash &other) const;

    private:
        bool m_valid;
        libtorrent::sha1_hash m_nativeHash;
        QString m_hashString;
    };

    uint qHash(const InfoHash &key, uint seed);
}

#endif // BITTORRENT_INFOHASH_H
