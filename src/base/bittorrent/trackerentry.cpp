#include <QString>

#include "base/utils/misc.h"
#include "base/utils/string.h"
#include "trackerentry.h"

using namespace BitTorrent;

TrackerEntry::TrackerEntry(const QString &url)
    : m_nativeEntry(libtorrent::announce_entry(url.toStdString()))
{
}

TrackerEntry::TrackerEntry(const libtorrent::announce_entry &nativeEntry)
    : m_nativeEntry(nativeEntry)
{
}

TrackerEntry::TrackerEntry(const TrackerEntry &other)
    : m_nativeEntry(other.m_nativeEntry)
{
}

QString TrackerEntry::url() const
{
    return QString::fromStdString(m_nativeEntry.url);
}

int TrackerEntry::tier() const
{
    return m_nativeEntry.tier;
}

TrackerEntry::Status TrackerEntry::status() const
{
    // libtorrent::announce_entry::is_working() returns
    // true when the tracker hasn't been tried yet.
    if (m_nativeEntry.verified && m_nativeEntry.is_working())
        return Working;
    else if ((m_nativeEntry.fails == 0) && m_nativeEntry.updating)
        return Updating;
    else if (m_nativeEntry.fails == 0)
        return NotContacted;
    else
        return NotWorking;
}

void TrackerEntry::setTier(int value)
{
    m_nativeEntry.tier = value;
}

TrackerEntry &TrackerEntry::operator=(const TrackerEntry &other)
{
    this->m_nativeEntry = other.m_nativeEntry;
    return *this;
}

bool TrackerEntry::operator==(const TrackerEntry &other) const
{
    return (QUrl(url()) == QUrl(other.url()));
}

libtorrent::announce_entry TrackerEntry::nativeEntry() const
{
    return m_nativeEntry;
}
