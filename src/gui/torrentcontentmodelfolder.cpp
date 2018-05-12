#include "torrentcontentmodelfolder.h"

#include "base/bittorrent/torrenthandle.h"

TorrentContentModelFolder::TorrentContentModelFolder(const QString &name, TorrentContentModelFolder *parent)
    : TorrentContentModelItem(parent)
{
    Q_ASSERT(parent);
    m_name = name;
    // Do not display incomplete extensions
    if (m_name.endsWith(QB_EXT))
        m_name.chop(4);
}

TorrentContentModelFolder::TorrentContentModelFolder(const QList<QVariant> &data)
    : TorrentContentModelItem(0)
{
    Q_ASSERT(data.size() == NB_COL);
    m_itemData = data;
}

TorrentContentModelFolder::~TorrentContentModelFolder()
{
    qDeleteAll(m_childItems);
}

TorrentContentModelItem::ItemType TorrentContentModelFolder::itemType() const
{
    return FolderType;
}

void TorrentContentModelFolder::deleteAllChildren()
{
    Q_ASSERT(isRootItem());
    qDeleteAll(m_childItems);
    m_childItems.clear();
}

const QList<TorrentContentModelItem *> &TorrentContentModelFolder::children() const
{
    return m_childItems;
}

void TorrentContentModelFolder::appendChild(TorrentContentModelItem *item)
{
    Q_ASSERT(item);
    m_childItems.append(item);
    // Update own size
    if (item->itemType() == FileType)
        increaseSize(item->size());
}

TorrentContentModelItem *TorrentContentModelFolder::child(int row) const
{
    return m_childItems.value(row, 0);
}

TorrentContentModelFolder *TorrentContentModelFolder::childFolderWithName(const QString &name) const
{
    foreach (TorrentContentModelItem *child, m_childItems)
        if ((child->itemType() == FolderType) && (child->name() == name))
            return static_cast<TorrentContentModelFolder *>(child);
    return 0;
}

int TorrentContentModelFolder::childCount() const
{
    return m_childItems.count();
}

// Only non-root folders use this function
void TorrentContentModelFolder::updatePriority()
{
    if (isRootItem())
        return;

    Q_ASSERT(!m_childItems.isEmpty());

    // If all children have the same priority
    // then the folder should have the same
    // priority
    const int prio = m_childItems.first()->priority();
    for (int i = 1; i < m_childItems.size(); ++i) {
        if (m_childItems.at(i)->priority() != prio) {
            setPriority(prio::MIXED);
            return;
        }
    }
    // All child items have the same priority
    // Update own if necessary
    setPriority(prio);
}

void TorrentContentModelFolder::setPriority(int newPriority, bool updateParent)
{
    if (m_priority == newPriority)
        return;

    m_priority = newPriority;

    // Update parent priority
    if (updateParent)
        m_parentItem->updatePriority();

    // Update children
    if (m_priority != prio::MIXED)
        foreach (TorrentContentModelItem *child, m_childItems)
            child->setPriority(m_priority, false);
}

void TorrentContentModelFolder::recalculateProgress()
{
    qreal tProgress = 0;
    qulonglong tSize = 0;
    qulonglong tRemaining = 0;
    foreach (TorrentContentModelItem *child, m_childItems) {
        if (child->priority() != prio::IGNORED) {
            if (child->itemType() == FolderType)
                static_cast<TorrentContentModelFolder *>(child)->recalculateProgress();
            tProgress += child->progress() * child->size();
            tSize += child->size();
            tRemaining += child->remaining();
        }
    }

    if (!isRootItem() && (tSize > 0)) {
        m_progress = tProgress / tSize;
        m_remaining = tRemaining;
        Q_ASSERT(m_progress <= 1.);
    }
}

void TorrentContentModelFolder::recalculateAvailability()
{
    qreal tAvailability = 0;
    qulonglong tSize = 0;
    bool foundAnyData = false;
    foreach (TorrentContentModelItem* child, m_childItems) {
        if (child->priority() == prio::IGNORED)
            continue;

        if (child->itemType() == FolderType)
            static_cast<TorrentContentModelFolder*>(child)->recalculateAvailability();
        const qreal childAvailability = child->availability();
        if (childAvailability >= 0) { // -1 means "no data"
            tAvailability += childAvailability * child->size();
            foundAnyData = true;
        }
        tSize += child->size();
    }

    if (!isRootItem() && (tSize > 0) && foundAnyData) {
        m_availability = tAvailability / tSize;
        Q_ASSERT(m_availability <= 1.);
    }
    else {
        m_availability = -1.;
    }
}

void TorrentContentModelFolder::increaseSize(qulonglong delta)
{
    if (isRootItem())
        return;

    m_size += delta;
    m_parentItem->increaseSize(delta);
}