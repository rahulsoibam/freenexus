#include "torrentcontentfiltermodel.h"

#include "base/utils/string.h"
#include "torrentcontentmodel.h"

TorrentContentFilterModel::TorrentContentFilterModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_model(new TorrentContentModel(this))
{
    connect(m_model, SIGNAL(filteredFilesChanged()), this, SIGNAL(filteredFilesChanged()));
    setSourceModel(m_model);
    // Filter settings
    setFilterKeyColumn(TorrentContentModelItem::COL_NAME);
    setFilterRole(Qt::DisplayRole);
    setDynamicSortFilter(true);
    setSortCaseSensitivity(Qt::CaseInsensitive);
}

TorrentContentFilterModel::~TorrentContentFilterModel()
{
    delete m_model;
}

TorrentContentModel *TorrentContentFilterModel::model() const
{
     return m_model;
}

TorrentContentModelItem::ItemType TorrentContentFilterModel::itemType(const QModelIndex &index) const
{
    return m_model->itemType(mapToSource(index));
}

int TorrentContentFilterModel::getFileIndex(const QModelIndex &index) const
{
    return m_model->getFileIndex(mapToSource(index));
}

QModelIndex TorrentContentFilterModel::parent(const QModelIndex &child) const
{
    if (!child.isValid()) return QModelIndex();

    QModelIndex sourceParent = m_model->parent(mapToSource(child));
    if (!sourceParent.isValid()) return QModelIndex();

    return mapFromSource(sourceParent);
}

bool TorrentContentFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if (m_model->itemType(m_model->index(sourceRow, 0, sourceParent)) == TorrentContentModelItem::FolderType) {
        // accept folders if they have at least one filtered item
        return hasFiltered(m_model->index(sourceRow, 0, sourceParent));
    }

    return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
}

bool TorrentContentFilterModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    switch (sortColumn()) {
    case TorrentContentModelItem::COL_NAME: {
            const TorrentContentModelItem::ItemType leftType = m_model->itemType(m_model->index(left.row(), 0, left.parent()));
            const TorrentContentModelItem::ItemType rightType = m_model->itemType(m_model->index(right.row(), 0, right.parent()));

            if (leftType == rightType) {
                const QString strL = left.data().toString();
                const QString strR = right.data().toString();
                return Utils::String::naturalLessThan<Qt::CaseInsensitive>(strL, strR);
            }
            else if ((leftType == TorrentContentModelItem::FolderType) && (sortOrder() == Qt::AscendingOrder)) {
                return true;
            }
            else {
                return false;
            }
        }
    default:
        return QSortFilterProxyModel::lessThan(left, right);
    };
}

void TorrentContentFilterModel::selectAll()
{
    for (int i = 0; i < rowCount(); ++i)
        setData(index(i, 0), Qt::Checked, Qt::CheckStateRole);

    emit dataChanged(index(0,0), index(rowCount(), columnCount()));
}

void TorrentContentFilterModel::selectNone()
{
    for (int i = 0; i < rowCount(); ++i)
        setData(index(i, 0), Qt::Unchecked, Qt::CheckStateRole);

    emit dataChanged(index(0,0), index(rowCount(), columnCount()));
}

bool TorrentContentFilterModel::hasFiltered(const QModelIndex &folder) const
{
    // this should be called only with folders
    // check if the folder name itself matches the filter string
    QString name = folder.data().toString();
    if (name.contains(filterRegExp()))
        return true;
    for (int child = 0; child < m_model->rowCount(folder); child++) {
        QModelIndex childIndex = m_model->index(child, 0, folder);
        if (m_model->hasChildren(childIndex)) {
            if (hasFiltered(childIndex))
                return true;
            else
                continue;
        }
        name = childIndex.data().toString();
        if (name.contains(filterRegExp()))
            return true;
    }

    return false;
}
