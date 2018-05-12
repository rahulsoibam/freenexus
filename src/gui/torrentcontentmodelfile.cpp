#include "torrentcontentmodelfile.h"

#include "base/bittorrent/torrenthandle.h"
#include "torrentcontentmodelfolder.h"

TorrentContentModelFile::TorrentContentModelFile(const QString &fileName, qulonglong fileSize,
                                                 TorrentContentModelFolder *parent, int fileIndex)
    : TorrentContentModelItem(parent)
    , m_fileIndex(fileIndex)
{
    Q_ASSERT(parent);

    m_name = fileName;

    // Do not display incomplete extensions
    if (m_name.endsWith(QB_EXT))
        m_name.chop(4);

    m_size = fileSize;
}

int TorrentContentModelFile::fileIndex() const
{
    return m_fileIndex;
}

void TorrentContentModelFile::setPriority(int newPriority, bool updateParent)
{
    Q_ASSERT(newPriority != prio::MIXED);

    if (m_priority == newPriority)
        return;

    m_priority = newPriority;

    // Update parent
    if (updateParent)
        m_parentItem->updatePriority();
}

void TorrentContentModelFile::setProgress(qreal progress)
{
    m_progress = progress;
    m_remaining = static_cast<qulonglong>(m_size * (1.0 - m_progress));
    Q_ASSERT(m_progress <= 1.);
}

void TorrentContentModelFile::setAvailability(qreal availability)
{
    m_availability = availability;
    Q_ASSERT(m_availability <= 1.);
}

TorrentContentModelItem::ItemType TorrentContentModelFile::itemType() const
{
    return FileType;
}
