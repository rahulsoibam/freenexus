#include "searchsortmodel.h"

SearchSortModel::SearchSortModel(QObject *parent)
    : base(parent)
    , m_isNameFilterEnabled(false)
    , m_minSeeds(0)
    , m_maxSeeds(-1)
    , m_minLeeches(0)
    , m_maxLeeches(-1)
    , m_minSize(0)
    , m_maxSize(-1)
{
}

void SearchSortModel::enableNameFilter(bool enabled)
{
    m_isNameFilterEnabled = enabled;
}

void SearchSortModel::setNameFilter(const QString &searchTerm)
{
    m_searchTerm = searchTerm;
    if (searchTerm.length() > 2
        && searchTerm.startsWith(QLatin1Char('"')) && searchTerm.endsWith(QLatin1Char('"'))) {
        m_searchTermWords = QStringList(m_searchTerm.mid(1, m_searchTerm.length() - 2));
    }
    else {
        m_searchTermWords = searchTerm.split(QLatin1Char(' '), QString::SkipEmptyParts);
    }
}

void SearchSortModel::setSizeFilter(qint64 minSize, qint64 maxSize)
{
    m_minSize = std::max(static_cast<qint64>(0), minSize);
    m_maxSize = std::max(static_cast<qint64>(-1), maxSize);
}

void SearchSortModel::setSeedsFilter(int minSeeds, int maxSeeds)
{
    m_minSeeds = std::max(0, minSeeds);
    m_maxSeeds = std::max(-1, maxSeeds);
}

void SearchSortModel::setLeechesFilter(int minLeeches, int maxLeeches)
{
    m_minLeeches = std::max(0, minLeeches);
    m_maxLeeches = std::max(-1, maxLeeches);
}

bool SearchSortModel::isNameFilterEnabled() const
{
    return m_isNameFilterEnabled;
}

QString SearchSortModel::searchTerm() const
{
    return m_searchTerm;
}

int SearchSortModel::minSeeds() const
{
    return m_minSeeds;
}

int SearchSortModel::maxSeeds() const
{
    return m_maxSeeds;
}

qint64 SearchSortModel::minSize() const
{
    return m_minSize;
}

qint64 SearchSortModel::maxSize() const
{
    return m_maxSize;
}

bool SearchSortModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    switch (sortColumn()) {
    case NAME:
    case ENGINE_URL: {
            const QString strL = left.data().toString();
            const QString strR = right.data().toString();
            const int result = Utils::String::naturalCompare(strL, strR, Qt::CaseInsensitive);
            return (result < 0);
    }
        break;
    default:
        return base::lessThan(left, right);
    };
}

bool SearchSortModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    const QAbstractItemModel* const sourceModel = this->sourceModel();
    if (m_isNameFilterEnabled && !m_searchTerm.isEmpty()) {
        QString name = sourceModel->data(sourceModel->index(sourceRow, NAME, sourceParent)).toString();
        for (const QString& word: m_searchTermWords) {
            int i = name.indexOf(word, 0, Qt::CaseInsensitive);
            if (i == -1) {
                return false;
            }
        }
    }

    if (m_minSize > 0 || m_maxSize >= 0) {
        qlonglong size = sourceModel->data(sourceModel->index(sourceRow, SIZE, sourceParent)).toLongLong();
        if ((m_minSize > 0 && size < m_minSize)
            ||  (m_maxSize > 0 && size > m_maxSize)) {
            return false;
        }
    }

    if (m_minSeeds > 0 || m_maxSeeds >= 0) {
        int seeds = sourceModel->data(sourceModel->index(sourceRow, SEEDS, sourceParent)).toInt();
        if ((m_minSeeds > 0 && seeds < m_minSeeds)
            ||  (m_maxSeeds > 0 && seeds > m_maxSeeds)) {
            return false;
        }
    }

    if (m_minLeeches > 0 || m_maxLeeches >= 0) {
        int leeches = sourceModel->data(sourceModel->index(sourceRow, LEECHES, sourceParent)).toInt();
        if ((m_minLeeches > 0 && leeches < m_minLeeches)
            ||  (m_maxLeeches > 0 && leeches > m_maxLeeches)) {
            return false;
        }
    }

    return base::filterAcceptsRow(sourceRow, sourceParent);
}
