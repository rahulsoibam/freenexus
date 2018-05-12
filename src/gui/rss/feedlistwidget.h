#ifndef FEEDLISTWIDGET_H
#define FEEDLISTWIDGET_H

#include <QHash>
#include <QTreeWidget>

namespace RSS
{
    class Article;
    class Feed;
    class Folder;
    class Item;
}

class FeedListWidget: public QTreeWidget
{
    Q_OBJECT

public:
    explicit FeedListWidget(QWidget *parent);
    ~FeedListWidget();

    QTreeWidgetItem *stickyUnreadItem() const;
    QList<QTreeWidgetItem *> getAllOpenedFolders(QTreeWidgetItem *parent = nullptr) const;
    RSS::Item *getRSSItem(QTreeWidgetItem *item) const;
    QTreeWidgetItem *mapRSSItem(RSS::Item *rssItem) const;
    QString itemPath(QTreeWidgetItem *item) const;
    bool isFeed(QTreeWidgetItem *item) const;
    bool isFolder(QTreeWidgetItem *item) const;

private slots:
    void handleItemAdded(RSS::Item *rssItem);
    void handleFeedStateChanged(RSS::Feed *feed);
    void handleFeedIconLoaded(RSS::Feed *feed);
    void handleItemUnreadCountChanged(RSS::Item *rssItem);
    void handleItemPathChanged(RSS::Item *rssItem);
    void handleItemAboutToBeRemoved(RSS::Item *rssItem);

private:
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    QTreeWidgetItem *createItem(RSS::Item *rssItem, QTreeWidgetItem *parentItem = nullptr);
    void fill(QTreeWidgetItem *parent, RSS::Folder *rssParent);

    QHash<RSS::Item *, QTreeWidgetItem *> m_rssToTreeItemMapping;
    QTreeWidgetItem *m_unreadStickyItem;
};

#endif // FEEDLISTWIDGET_H
