#ifndef SEARCHSORTMODEL_H
#define SEARCHSORTMODEL_H

#include <QSortFilterProxyModel>
#include <QStringList>
#include "base/utils/string.h"

class SearchSortModel: public QSortFilterProxyModel
{
    using base = QSortFilterProxyModel;

public:
    enum SearchColumn
    {
        NAME,
        SIZE,
        SEEDS,
        LEECHES,
        ENGINE_URL,
        DL_LINK,
        DESC_LINK,
        NB_SEARCH_COLUMNS
    };

    explicit SearchSortModel(QObject *parent = 0);

    void enableNameFilter(bool enabled);
    void setNameFilter(const QString& searchTerm = QString());

    //! \brief Sets parameters for filtering by size
    //! \param minSize minimal size in bytes
    //! \param maxSize maximal size in bytes, negative value to disable filtering
    void setSizeFilter(qint64 minSize, qint64 maxSize);

    //! \brief Sets parameters for filtering by seeds number
    //! \param minSeeds minimal number of seeders
    //! \param maxSeeds maximal number of seeders, negative value to disable filtering
    void setSeedsFilter(int minSeeds, int maxSeeds);

    //! \brief Sets parameters for filtering by leeches number
    //! \param minLeeches minimal number of leechers
    //! \param maxLeeches maximal number of leechers, negative value to disable filtering
    void setLeechesFilter(int minLeeches, int maxLeeches);

    bool isNameFilterEnabled() const;

    QString searchTerm() const;

    int minSeeds() const;
    int maxSeeds() const;

    qint64 minSize() const;
    qint64 maxSize() const;

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    bool m_isNameFilterEnabled;
    QString m_searchTerm;
    QStringList m_searchTermWords;
    int m_minSeeds, m_maxSeeds;
    int m_minLeeches, m_maxLeeches;
    qint64 m_minSize, m_maxSize;
};

#endif // SEARCHSORTMODEL_H
