#ifndef TORRENTCONTENTMODEL_H
#define TORRENTCONTENTMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVector>
#include <QVariant>

#include "base/bittorrent/torrentinfo.h"
#include "torrentcontentmodelitem.h"

class QFileIconProvider;
class TorrentContentModelFile;

class TorrentContentModel: public QAbstractItemModel
{
    Q_OBJECT

public:
    TorrentContentModel(QObject *parent = 0);
    ~TorrentContentModel();

    void updateFilesProgress(const QVector<qreal> &fp);
    void updateFilesPriorities(const QVector<int> &fprio);
    void updateFilesAvailability(const QVector<qreal> &fa);
    QVector<int> getFilePriorities() const;
    bool allFiltered() const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    TorrentContentModelItem::ItemType itemType(const QModelIndex& index) const;
    int getFileIndex(const QModelIndex& index);
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    void clear();
    void setupModelData(const BitTorrent::TorrentInfo &info);

signals:
    void filteredFilesChanged();

public slots:
    void selectAll();
    void selectNone();

private:
    TorrentContentModelFolder *m_rootItem;
    QVector<TorrentContentModelFile *> m_filesIndex;
    QFileIconProvider *m_fileIconProvider;
};

#endif // TORRENTCONTENTMODEL_H
