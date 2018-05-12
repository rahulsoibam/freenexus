#ifndef TORRENTCONTENTMODELITEM_H
#define TORRENTCONTENTMODELITEM_H

#include <QList>
#include <QVariant>

namespace prio
{
    enum FilePriority
    {
        IGNORED=0,
        NORMAL=1,
        HIGH=6,
        MAXIMUM=7,
        MIXED=-1
    };
}

class TorrentContentModelFolder;

class TorrentContentModelItem
{
public:
    enum TreeItemColumns
    {
        COL_NAME,
        COL_SIZE,
        COL_PROGRESS,
        COL_PRIO,
        COL_REMAINING,
        COL_AVAILABILITY,
        NB_COL
    };

    enum ItemType
    {
        FileType,
        FolderType
    };

    TorrentContentModelItem(TorrentContentModelFolder *parent);
    virtual ~TorrentContentModelItem();

    bool isRootItem() const;
    TorrentContentModelFolder *parent() const;
    virtual ItemType itemType() const = 0;

    QString name() const;
    void setName(const QString &name);

    qulonglong size() const;
    qreal progress() const;
    qulonglong remaining() const;

    qreal availability() const;

    int priority() const;
    virtual void setPriority(int newPriority, bool updateParent = true) = 0;

    int columnCount() const;
    QVariant data(int column) const;
    int row() const;

protected:
    TorrentContentModelFolder *m_parentItem;
    // Root item members
    QList<QVariant> m_itemData;
    // Non-root item members
    QString m_name;
    qulonglong m_size;
    qulonglong m_remaining;
    int m_priority;
    qreal m_progress;
    qreal m_availability;
};

#endif // TORRENTCONTENTMODELITEM_H
