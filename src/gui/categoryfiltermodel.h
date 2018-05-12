#ifndef CATEGORYFILTERMODEL_H
#define CATEGORYFILTERMODEL_H

#include <QAbstractItemModel>
#include <QHash>
#include <QModelIndex>

namespace BitTorrent
{
    class TorrentHandle;
}

class CategoryModelItem;

class CategoryFilterModel: public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit CategoryFilterModel(QObject *parent = nullptr);
    ~CategoryFilterModel();

    static bool isSpecialItem(const QModelIndex &index);

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex index(const QString &categoryName) const;
    QString categoryName(const QModelIndex &index) const;

private slots:
    void categoryAdded(const QString &categoryName);
    void categoryRemoved(const QString &categoryName);
    void torrentAdded(BitTorrent::TorrentHandle *const torrent);
    void torrentAboutToBeRemoved(BitTorrent::TorrentHandle *const torrent);
    void torrentCategoryChanged(BitTorrent::TorrentHandle *const torrent, const QString &oldCategory);
    void subcategoriesSupportChanged();

private:
    void populate();
    QModelIndex index(CategoryModelItem *item) const;
    CategoryModelItem *findItem(const QString &fullName) const;

    bool m_isSubcategoriesEnabled;
    CategoryModelItem *m_rootItem;
};

#endif // CATEGORYFILTERMODEL_H
