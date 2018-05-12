#ifndef TAGFILTERMODEL_H
#define TAGFILTERMODEL_H

#include <QAbstractListModel>
#include <QHash>
#include <QModelIndex>
#include <QSet>
#include <QVector>

namespace BitTorrent
{
    class TorrentHandle;
}

class TagModelItem;

class TagFilterModel: public QAbstractListModel
{
    Q_OBJECT

public:
    explicit TagFilterModel(QObject *parent = nullptr);
    ~TagFilterModel();

    static bool isSpecialItem(const QModelIndex &index);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex index(const QString &tag) const;
    QString tag(const QModelIndex &index) const;

private slots:
    void tagAdded(const QString &tag);
    void tagRemoved(const QString &tag);
    void torrentTagAdded(BitTorrent::TorrentHandle *const torrent, const QString &tag);
    void torrentTagRemoved(BitTorrent::TorrentHandle *const, const QString &tag);
    void torrentAdded(BitTorrent::TorrentHandle *const torrent);
    void torrentAboutToBeRemoved(BitTorrent::TorrentHandle *const torrent);

private:
    static QString tagDisplayName(const QString &tag);

    void populate();
    void addToModel(const QString &tag, int count);
    void removeFromModel(int row);
    bool isValidRow(int row) const;
    int findRow(const QString &tag) const;
    TagModelItem *findItem(const QString &tag);
    QVector<TagModelItem *> findItems(const QSet<QString> &tags);
    TagModelItem *allTagsItem();
    TagModelItem *untaggedItem();

    QList<TagModelItem> m_tagItems;  // Index corresponds to its row
};

#endif // TAGFILTERMODEL_H
