#ifndef TORRENTCONTENTFILTERMODEL_H
#define TORRENTCONTENTFILTERMODEL_H

#include <QSortFilterProxyModel>

#include "torrentcontentmodelitem.h"

class TorrentContentModel;

class TorrentContentFilterModel: public QSortFilterProxyModel
{
    Q_OBJECT

public:
    TorrentContentFilterModel(QObject *parent = 0);
    virtual ~TorrentContentFilterModel();

    TorrentContentModel *model() const;
    TorrentContentModelItem::ItemType itemType(const QModelIndex &index) const;
    int getFileIndex(const QModelIndex &index) const;
    virtual QModelIndex parent(const QModelIndex &child) const;

public slots:
    void selectAll();
    void selectNone();

signals:
    void filteredFilesChanged();

protected:
    virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
    virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

private:
    TorrentContentModel *m_model;
    bool hasFiltered(const QModelIndex &folder) const;
};

#endif // TORRENTCONTENTFILTERMODEL_H
