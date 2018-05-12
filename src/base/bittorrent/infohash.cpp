#include <QHash>
#include "infohash.h"

using namespace BitTorrent;

InfoHash::InfoHash()
    : m_valid(false)
{
}

InfoHash::InfoHash(const libtorrent::sha1_hash &nativeHash)
    : m_valid(true)
    , m_nativeHash(nativeHash)
{
    char out[(libtorrent::sha1_hash::size * 2) + 1];
    libtorrent::to_hex(reinterpret_cast<const char*>(&m_nativeHash[0]), libtorrent::sha1_hash::size, out);
    m_hashString = QString(out);
}

InfoHash::InfoHash(const QString &hashString)
    : m_valid(false)
    , m_hashString(hashString)
{
    QByteArray raw = m_hashString.toLatin1();
    if (raw.size() == 40)
        m_valid = libtorrent::from_hex(raw.constData(), 40, reinterpret_cast<char*>(&m_nativeHash[0]));
}


InfoHash::InfoHash(const InfoHash &other)
    : m_valid(other.m_valid)
    , m_nativeHash(other.m_nativeHash)
    , m_hashString(other.m_hashString)
{
}


bool InfoHash::isValid() const
{
    return m_valid;
}

InfoHash::operator libtorrent::sha1_hash() const
{
    return m_nativeHash;
}


InfoHash::operator QString() const
{
    return m_hashString;
}


bool InfoHash::operator==(const InfoHash &other) const
{
    return (m_nativeHash == other.m_nativeHash);
}


bool InfoHash::operator!=(const InfoHash &other) const
{
    return (m_nativeHash != other.m_nativeHash);
}

uint BitTorrent::qHash(const InfoHash &key, uint seed)
{
    return qHash(static_cast<QString>(key), seed);
}
