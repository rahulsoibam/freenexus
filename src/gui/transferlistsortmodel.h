#ifndef TRANSFERLISTSORTMODEL_H
#define TRANSFERLISTSORTMODEL_H

#include <QSortFilterProxyModel>
#include "base/torrentfilter.h"

class QStringList;

class TransferListSortModel: public QSortFilterProxyModel
{
    Q_OBJECT

public:
    TransferListSortModel(QObject *parent = 0);

    void setStatusFilter(TorrentFilter::Type filter);
    void setCategoryFilter(const QString &category);
    void disableCategoryFilter();
    void setTagFilter(const QString &tag);
    void disableTagFilter();
    void setTrackerFilter(const QStringList &hashes);
    void disableTrackerFilter();

private:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
    bool lowerPositionThan(const QModelIndex &left, const QModelIndex &right) const;
    bool dateLessThan(const int dateColumn, const QModelIndex &left, const QModelIndex &right, bool sortInvalidInBottom) const;
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
    bool matchFilter(int sourceRow, const QModelIndex &sourceParent) const;

private:
    TorrentFilter m_filter;
};

#endif // TRANSFERLISTSORTMODEL_H
