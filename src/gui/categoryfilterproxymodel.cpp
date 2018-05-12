#include "categoryfilterproxymodel.h"

#include "base/utils/string.h"
#include "categoryfiltermodel.h"

CategoryFilterProxyModel::CategoryFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

QModelIndex CategoryFilterProxyModel::index(const QString &categoryName) const
{
    return mapFromSource(static_cast<CategoryFilterModel *>(sourceModel())->index(categoryName));
}

QString CategoryFilterProxyModel::categoryName(const QModelIndex &index) const
{
    return static_cast<CategoryFilterModel *>(sourceModel())->categoryName(mapToSource(index));
}

bool CategoryFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    // "All" and "Uncategorized" must be left in place
    if (CategoryFilterModel::isSpecialItem(left) || CategoryFilterModel::isSpecialItem(right))
        return (left < right);

    int result = Utils::String::naturalCompare(left.data().toString(), right.data().toString()
        , Qt::CaseInsensitive);

    return (result < 0);
}
