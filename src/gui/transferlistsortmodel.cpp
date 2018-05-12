#include <QStringList>

#include "base/types.h"
#include "base/utils/string.h"
#include "base/bittorrent/torrenthandle.h"
#include "torrentmodel.h"
#include "transferlistsortmodel.h"

TransferListSortModel::TransferListSortModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

void TransferListSortModel::setStatusFilter(TorrentFilter::Type filter)
{
    if (m_filter.setType(filter))
        invalidateFilter();
}

void TransferListSortModel::setCategoryFilter(const QString &category)
{
    if (m_filter.setCategory(category))
        invalidateFilter();
}

void TransferListSortModel::disableCategoryFilter()
{
    if (m_filter.setCategory(TorrentFilter::AnyCategory))
        invalidateFilter();
}

void TransferListSortModel::setTagFilter(const QString &tag)
{
    if (m_filter.setTag(tag))
        invalidateFilter();
}

void TransferListSortModel::disableTagFilter()
{
    if (m_filter.setTag(TorrentFilter::AnyTag))
        invalidateFilter();
}

void TransferListSortModel::setTrackerFilter(const QStringList &hashes)
{
    if (m_filter.setHashSet(hashes.toSet()))
        invalidateFilter();
}

void TransferListSortModel::disableTrackerFilter()
{
    if (m_filter.setHashSet(TorrentFilter::AnyHash))
        invalidateFilter();
}

bool TransferListSortModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    switch (sortColumn()) {
    case TorrentModel::TR_CATEGORY:
    case TorrentModel::TR_TAGS:
    case TorrentModel::TR_NAME: {
        const QVariant vL = left.data();
        const QVariant vR = right.data();
        if (!vL.isValid() || !vR.isValid() || (vL == vR))
            return lowerPositionThan(left, right);

        const int result = Utils::String::naturalCompare(vL.toString(), vR.toString(), Qt::CaseInsensitive);
        return (result < 0);
    }

    case TorrentModel::TR_STATUS: {
        // QSortFilterProxyModel::lessThan() uses the < operator only for specific QVariant types
        // so our custom type is outside that list.
        // In this case QSortFilterProxyModel::lessThan() converts other types to QString and
        // sorts them.
        // Thus we can't use the code in the default label.
        const BitTorrent::TorrentState leftValue = left.data().value<BitTorrent::TorrentState>();
        const BitTorrent::TorrentState rightValue = right.data().value<BitTorrent::TorrentState>();
        if (leftValue != rightValue)
            return leftValue < rightValue;

        return lowerPositionThan(left, right);
    }

    case TorrentModel::TR_ADD_DATE:
    case TorrentModel::TR_SEED_DATE:
    case TorrentModel::TR_SEEN_COMPLETE_DATE: {
        return dateLessThan(sortColumn(), left, right, true);
    }

    case TorrentModel::TR_PRIORITY: {
        return lowerPositionThan(left, right);
    }

    case TorrentModel::TR_SEEDS:
    case TorrentModel::TR_PEERS: {
        const int leftActive = left.data().toInt();
        const int leftTotal = left.data(Qt::UserRole).toInt();
        const int rightActive = right.data().toInt();
        const int rightTotal = right.data(Qt::UserRole).toInt();

        // Active peers/seeds take precedence over total peers/seeds.
        if (leftActive != rightActive)
            return (leftActive < rightActive);

        if (leftTotal != rightTotal)
            return (leftTotal < rightTotal);

        return lowerPositionThan(left, right);
    }

    case TorrentModel::TR_ETA: {
        const TorrentModel *model = qobject_cast<TorrentModel *>(sourceModel());

        // Sorting rules prioritized.
        // 1. Active torrents at the top
        // 2. Seeding torrents at the bottom
        // 3. Torrents with invalid ETAs at the bottom

        const bool isActiveL = TorrentFilter::ActiveTorrent.match(model->torrentHandle(model->index(left.row())));
        const bool isActiveR = TorrentFilter::ActiveTorrent.match(model->torrentHandle(model->index(right.row())));
        if (isActiveL != isActiveR)
            return isActiveL;

        const int prioL = model->data(model->index(left.row(), TorrentModel::TR_PRIORITY)).toInt();
        const int prioR = model->data(model->index(right.row(), TorrentModel::TR_PRIORITY)).toInt();
        const bool isSeedingL = (prioL < 0);
        const bool isSeedingR = (prioR < 0);
        if (isSeedingL != isSeedingR) {
            const bool isAscendingOrder = (sortOrder() == Qt::AscendingOrder);
            if (isSeedingL)
                return !isAscendingOrder;
            else
                return isAscendingOrder;
        }

        const qlonglong etaL = left.data().toLongLong();
        const qlonglong etaR = right.data().toLongLong();
        const bool isInvalidL = ((etaL < 0) || (etaL >= MAX_ETA));
        const bool isInvalidR = ((etaR < 0) || (etaR >= MAX_ETA));
        if (isInvalidL && isInvalidR) {
            if (isSeedingL)  // Both seeding
                return dateLessThan(TorrentModel::TR_SEED_DATE, left, right, true);
            else
                return (prioL < prioR);
        }
        else if (!isInvalidL && !isInvalidR) {
            return (etaL < etaR);
        }
        else {
            return !isInvalidL;
        }
    }

    case TorrentModel::TR_LAST_ACTIVITY: {
        const qlonglong vL = left.data().toLongLong();
        const qlonglong vR = right.data().toLongLong();

        if (vL == -1) return false;
        if (vR == -1) return true;

        return vL < vR;
    }

    case TorrentModel::TR_RATIO_LIMIT: {
        const qreal vL = left.data().toDouble();
        const qreal vR = right.data().toDouble();

        if (vL == -1) return false;
        if (vR == -1) return true;

        return vL < vR;
    }

    default: {
        if (left.data() != right.data())
            return QSortFilterProxyModel::lessThan(left, right);

        return lowerPositionThan(left, right);
    }
    }
}

bool TransferListSortModel::lowerPositionThan(const QModelIndex &left, const QModelIndex &right) const
{
    const TorrentModel *model = qobject_cast<TorrentModel *>(sourceModel());

    // Sort according to TR_PRIORITY
    const int queueL = model->data(model->index(left.row(), TorrentModel::TR_PRIORITY)).toInt();
    const int queueR = model->data(model->index(right.row(), TorrentModel::TR_PRIORITY)).toInt();
    if ((queueL > 0) || (queueR > 0)) {
        if ((queueL > 0) && (queueR > 0))
            return queueL < queueR;
        else
            return queueL != 0;
    }

    // Sort according to TR_SEED_DATE
    return dateLessThan(TorrentModel::TR_SEED_DATE, left, right, false);
}

// Every time we compare QDateTimes we need a fallback comparison in case both
// values are empty. This is a workaround for unstable sort in QSortFilterProxyModel
// (detailed discussion in #2526 and #2158).
bool TransferListSortModel::dateLessThan(const int dateColumn, const QModelIndex &left, const QModelIndex &right, bool sortInvalidInBottom) const
{
    const TorrentModel *model = qobject_cast<TorrentModel *>(sourceModel());
    const QDateTime dateL = model->data(model->index(left.row(), dateColumn)).toDateTime();
    const QDateTime dateR = model->data(model->index(right.row(), dateColumn)).toDateTime();
    if (dateL.isValid() && dateR.isValid()) {
        if (dateL != dateR)
            return dateL < dateR;
    }
    else if (dateL.isValid()) {
        return sortInvalidInBottom;
    }
    else if (dateR.isValid()) {
        return !sortInvalidInBottom;
    }

    // Finally, sort by hash
    const QString hashL(model->torrentHandle(model->index(left.row()))->hash());
    const QString hashR(model->torrentHandle(model->index(right.row()))->hash());
    return hashL < hashR;
}

bool TransferListSortModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    return matchFilter(sourceRow, sourceParent)
           && QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
}

bool TransferListSortModel::matchFilter(int sourceRow, const QModelIndex &sourceParent) const
{
    TorrentModel *model = qobject_cast<TorrentModel *>(sourceModel());
    if (!model) return false;

    BitTorrent::TorrentHandle *const torrent = model->torrentHandle(model->index(sourceRow, 0, sourceParent));
    if (!torrent) return false;

    return m_filter.match(torrent);
}
