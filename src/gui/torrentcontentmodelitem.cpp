#include "base/utils/misc.h"
#include "base/utils/fs.h"
#include "torrentcontentmodelitem.h"
#include "torrentcontentmodelfolder.h"
#include <QDebug>

TorrentContentModelItem::TorrentContentModelItem(TorrentContentModelFolder *parent)
    : m_parentItem(parent)
    , m_size(0)
    , m_remaining(0)
    , m_priority(prio::NORMAL)
    , m_progress(0)
    , m_availability(-1.)
{
}

TorrentContentModelItem::~TorrentContentModelItem() = default;

bool TorrentContentModelItem::isRootItem() const
{
    return !m_parentItem;
}

QString TorrentContentModelItem::name() const
{
    Q_ASSERT(!isRootItem());
    return m_name;
}

void TorrentContentModelItem::setName(const QString &name)
{
    Q_ASSERT(!isRootItem());
    m_name = name;
}

qulonglong TorrentContentModelItem::size() const
{
    Q_ASSERT(!isRootItem());

    return m_size;
}

qreal TorrentContentModelItem::progress() const
{
    Q_ASSERT(!isRootItem());

    if (m_size > 0) return m_progress;

    return 1;
}

qulonglong TorrentContentModelItem::remaining() const
{
    Q_ASSERT(!isRootItem());
    return m_remaining;
}

qreal TorrentContentModelItem::availability() const
{
    Q_ASSERT(!isRootItem());

    return m_size > 0 ? m_availability : 0.;
}

int TorrentContentModelItem::priority() const
{
    Q_ASSERT(!isRootItem());
    return m_priority;
}

int TorrentContentModelItem::columnCount() const
{
    return NB_COL;
}

QVariant TorrentContentModelItem::data(int column) const
{
    if (isRootItem())
        return m_itemData.value(column);

    switch (column) {
    case COL_NAME:
        return m_name;
    case COL_PRIO:
        return m_priority;
    case COL_PROGRESS:
        return progress();
    case COL_SIZE:
        return m_size;
    case COL_REMAINING:
        return remaining();
    case COL_AVAILABILITY:
        return availability();
    default:
        Q_ASSERT(false);
        return QVariant();
    }
}

int TorrentContentModelItem::row() const
{
    if (m_parentItem)
        return m_parentItem->children().indexOf(const_cast<TorrentContentModelItem *>(this));
    return 0;
}

TorrentContentModelFolder *TorrentContentModelItem::parent() const
{
    return m_parentItem;
}

