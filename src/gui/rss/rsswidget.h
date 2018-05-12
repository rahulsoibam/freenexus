#ifndef RSSWIDGET_H
#define RSSWIDGET_H

#include <QPointer>
#include <QShortcut>

class ArticleListWidget;
class FeedListWidget;
class QListWidgetItem;
class QTreeWidgetItem;

namespace Ui
{
    class RSSWidget;
}

class RSSWidget: public QWidget
{
    Q_OBJECT

public:
    RSSWidget(QWidget *parent);
    ~RSSWidget();

public slots:
    void deleteSelectedItems();
    void updateRefreshInterval(uint val);

signals:
    void unreadCountUpdated(int count);

private slots:
    void on_newFeedButton_clicked();
    void refreshAllFeeds();
    void on_markReadButton_clicked();
    void displayRSSListMenu(const QPoint &);
    void displayItemsListMenu(const QPoint &);
    void renameSelectedRSSItem();
    void refreshSelectedItems();
    void copySelectedFeedsURL();
    void handleCurrentFeedItemChanged(QTreeWidgetItem *currentItem);
    void handleCurrentArticleItemChanged(QListWidgetItem *currentItem, QListWidgetItem *previousItem);
    void openSelectedArticlesUrls();
    void downloadSelectedTorrents();
    void saveSlidersPosition();
    void restoreSlidersPosition();
    void askNewFolder();
    void saveFoldersOpenState();
    void loadFoldersOpenState();
    void on_rssDownloaderBtn_clicked();
    void handleSessionProcessingStateChanged(bool enabled);
    void handleUnreadCountChanged();

private:
    Ui::RSSWidget *m_ui;
    ArticleListWidget *m_articleListWidget;
    FeedListWidget *m_feedListWidget;
    QShortcut *m_editHotkey;
    QShortcut *m_deleteHotkey;
};

#endif // RSSWIDGET_H
