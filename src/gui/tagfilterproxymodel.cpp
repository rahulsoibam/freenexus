#include "tagfilterproxymodel.h"

#include "base/utils/string.h"
#include "tagfiltermodel.h"

TagFilterProxyModel::TagFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

QModelIndex TagFilterProxyModel::index(const QString &tag) const
{
    return mapFromSource(static_cast<TagFilterModel *>(sourceModel())->index(tag));
}

QString TagFilterProxyModel::tag(const QModelIndex &index) const
{
    return static_cast<TagFilterModel *>(sourceModel())->tag(mapToSource(index));
}

bool TagFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    // "All" and "Untagged" must be left in place
    if (TagFilterModel::isSpecialItem(left) || TagFilterModel::isSpecialItem(right))
        return (left < right);

    int result = Utils::String::naturalCompare(left.data().toString(), right.data().toString()
        , Qt::CaseInsensitive);
    return (result < 0);
}
