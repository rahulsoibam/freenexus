#include "rss_folder.h"

#include <QJsonObject>
#include <QJsonValue>

#include "base/global.h"
#include "rss_article.h"

using namespace RSS;

Folder::Folder(const QString &path)
    : Item(path)
{
}

Folder::~Folder()
{
    emit aboutToBeDestroyed(this);

    foreach (auto item, items())
        delete item;
}

QList<Article *> Folder::articles() const
{
    QList<Article *> news;

    foreach (Item *item, items()) {
        int n = news.size();
        news << item->articles();
        std::inplace_merge(news.begin(), news.begin() + n, news.end()
                           , [](Article *a1, Article *a2)
        {
            return Article::articleDateRecentThan(a1, a2->date());
        });
    }
    return news;
}

int Folder::unreadCount() const
{
    int count = 0;
    foreach (Item *item, items())
        count += item->unreadCount();
    return count;
}

void Folder::markAsRead()
{
    foreach (Item *item, items())
        item->markAsRead();
}

void Folder::refresh()
{
    foreach (Item *item, items())
        item->refresh();
}

QList<Item *> Folder::items() const
{
    return m_items;
}

QJsonValue Folder::toJsonValue(bool withData) const
{
    QJsonObject jsonObj;
    foreach (Item *item, items())
        jsonObj.insert(item->name(), item->toJsonValue(withData));

    return jsonObj;
}

void Folder::handleItemUnreadCountChanged()
{
    emit unreadCountChanged(this);
}

void Folder::cleanup()
{
    foreach (Item *item, items())
        item->cleanup();
}

void Folder::addItem(Item *item)
{
    Q_ASSERT(item);
    Q_ASSERT(!m_items.contains(item));

    m_items.append(item);
    connect(item, &Item::newArticle, this, &Item::newArticle);
    connect(item, &Item::articleRead, this, &Item::articleRead);
    connect(item, &Item::articleAboutToBeRemoved, this, &Item::articleAboutToBeRemoved);
    connect(item, &Item::unreadCountChanged, this, &Folder::handleItemUnreadCountChanged);

    for (auto article: copyAsConst(item->articles()))
        emit newArticle(article);

    if (item->unreadCount() > 0)
        emit unreadCountChanged(this);
}

void Folder::removeItem(Item *item)
{
    Q_ASSERT(m_items.contains(item));

    for (auto article: copyAsConst(item->articles()))
        emit articleAboutToBeRemoved(article);

    item->disconnect(this);
    m_items.removeOne(item);
    if (item->unreadCount() > 0)
        emit unreadCountChanged(this);
}
