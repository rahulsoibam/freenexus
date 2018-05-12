#ifndef TORRENTCONTENTMODELFOLDER_H
#define TORRENTCONTENTMODELFOLDER_H

#include "torrentcontentmodelitem.h"

class TorrentContentModelFolder: public TorrentContentModelItem
{
public:
    // Folder constructor
    TorrentContentModelFolder(const QString& name, TorrentContentModelFolder* parent);

    // Invisible root item constructor
    TorrentContentModelFolder(const QList<QVariant>& data);

    ~TorrentContentModelFolder();

    ItemType itemType() const override;

    void increaseSize(qulonglong delta);
    void recalculateProgress();
    void recalculateAvailability();
    void updatePriority();

    void setPriority(int newPriority, bool updateParent = true) override;

    void deleteAllChildren();
    const QList<TorrentContentModelItem*>& children() const;
    void appendChild(TorrentContentModelItem* item);
    TorrentContentModelItem* child(int row) const;
    TorrentContentModelFolder* childFolderWithName(const QString& name) const;
    int childCount() const;

private:
    QList<TorrentContentModelItem*> m_childItems;
};

#endif // TORRENTCONTENTMODELFOLDER_H
