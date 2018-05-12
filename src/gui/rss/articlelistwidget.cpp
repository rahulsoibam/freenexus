#include "articlelistwidget.h"

#include <QListWidgetItem>

#include "base/rss/rss_article.h"
#include "base/rss/rss_item.h"

ArticleListWidget::ArticleListWidget(QWidget *parent)
    : QListWidget(parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    setSelectionMode(QAbstractItemView::ExtendedSelection);

    checkInvariant();
}

RSS::Article *ArticleListWidget::getRSSArticle(QListWidgetItem *item) const
{
    Q_ASSERT(item);
    return reinterpret_cast<RSS::Article *>(item->data(Qt::UserRole).value<quintptr>());
}

QListWidgetItem *ArticleListWidget::mapRSSArticle(RSS::Article *rssArticle) const
{
    return m_rssArticleToListItemMapping.value(rssArticle);
}

void ArticleListWidget::setRSSItem(RSS::Item *rssItem, bool unreadOnly)
{
    // Clear the list first
    clear();
    m_rssArticleToListItemMapping.clear();
    if (m_rssItem)
        m_rssItem->disconnect(this);

    m_unreadOnly = unreadOnly;
    m_rssItem = rssItem;
    if (m_rssItem) {
        connect(m_rssItem, &RSS::Item::newArticle, this, &ArticleListWidget::handleArticleAdded);
        connect(m_rssItem, &RSS::Item::articleRead, this, &ArticleListWidget::handleArticleRead);
        connect(m_rssItem, &RSS::Item::articleAboutToBeRemoved, this, &ArticleListWidget::handleArticleAboutToBeRemoved);

        foreach (auto article, rssItem->articles()) {
            if (!(m_unreadOnly && article->isRead())) {
                auto item = createItem(article);
                addItem(item);
                m_rssArticleToListItemMapping.insert(article, item);
            }
        }
    }

    checkInvariant();
}

void ArticleListWidget::handleArticleAdded(RSS::Article *rssArticle)
{
    if (!(m_unreadOnly && rssArticle->isRead())) {
        auto item = createItem(rssArticle);
        insertItem(0, item);
        m_rssArticleToListItemMapping.insert(rssArticle, item);
    }

    checkInvariant();
}

void ArticleListWidget::handleArticleRead(RSS::Article *rssArticle)
{
    auto item = mapRSSArticle(rssArticle);
    if (!item) return;

    item->setData(Qt::ForegroundRole, QPalette().color(QPalette::Inactive, QPalette::WindowText));
    item->setData(Qt::DecorationRole, QIcon(":/icons/sphere.png"));

    checkInvariant();
}

void ArticleListWidget::handleArticleAboutToBeRemoved(RSS::Article *rssArticle)
{
    delete m_rssArticleToListItemMapping.take(rssArticle);
    checkInvariant();
}

void ArticleListWidget::checkInvariant() const
{
    Q_ASSERT(count() == m_rssArticleToListItemMapping.count());
}

QListWidgetItem *ArticleListWidget::createItem(RSS::Article *article) const
{
    Q_ASSERT(article);
    QListWidgetItem *item = new QListWidgetItem;

    item->setData(Qt::DisplayRole, article->title());
    item->setData(Qt::UserRole, reinterpret_cast<quintptr>(article));
    if (article->isRead()) {
        item->setData(Qt::ForegroundRole, QPalette().color(QPalette::Inactive, QPalette::WindowText));
        item->setData(Qt::DecorationRole, QIcon(":/icons/sphere.png"));
    }
    else {
        item->setData(Qt::ForegroundRole, QPalette().color(QPalette::Active, QPalette::Link));
        item->setData(Qt::DecorationRole, QIcon(":/icons/sphere2.png"));
    }

    return item;
}
