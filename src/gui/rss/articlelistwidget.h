#ifndef ARTICLELISTWIDGET_H
#define ARTICLELISTWIDGET_H

#include <QHash>
#include <QListWidget>

namespace RSS
{
    class Article;
    class Item;
}

class ArticleListWidget: public QListWidget
{
    Q_OBJECT

public:
    explicit ArticleListWidget(QWidget *parent);

    RSS::Article *getRSSArticle(QListWidgetItem *item) const;
    QListWidgetItem *mapRSSArticle(RSS::Article *rssArticle) const;

    void setRSSItem(RSS::Item *rssItem, bool unreadOnly = false);

private slots:
    void handleArticleAdded(RSS::Article *rssArticle);
    void handleArticleRead(RSS::Article *rssArticle);
    void handleArticleAboutToBeRemoved(RSS::Article *rssArticle);

private:
    void checkInvariant() const;
    QListWidgetItem *createItem(RSS::Article *article) const;

    RSS::Item *m_rssItem = nullptr;
    bool m_unreadOnly = false;
    QHash<RSS::Article *, QListWidgetItem *> m_rssArticleToListItemMapping;
};

#endif // ARTICLELISTWIDGET_H
