#include "rss_article.h"

#include <stdexcept>
#include <QJsonObject>
#include <QVariant>

#include "rss_feed.h"

using namespace RSS;

const QString Article::KeyId(QStringLiteral("id"));
const QString Article::KeyDate(QStringLiteral("date"));
const QString Article::KeyTitle(QStringLiteral("title"));
const QString Article::KeyAuthor(QStringLiteral("author"));
const QString Article::KeyDescription(QStringLiteral("description"));
const QString Article::KeyTorrentURL(QStringLiteral("torrentURL"));
const QString Article::KeyLink(QStringLiteral("link"));
const QString Article::KeyIsRead(QStringLiteral("isRead"));

Article::Article(Feed *feed, const QVariantHash &varHash)
    : QObject(feed)
    , m_feed(feed)
    , m_guid(varHash.value(KeyId).toString())
    , m_date(varHash.value(KeyDate).toDateTime())
    , m_title(varHash.value(KeyTitle).toString())
    , m_author(varHash.value(KeyAuthor).toString())
    , m_description(varHash.value(KeyDescription).toString())
    , m_torrentURL(varHash.value(KeyTorrentURL).toString())
    , m_link(varHash.value(KeyLink).toString())
    , m_isRead(varHash.value(KeyIsRead, false).toBool())
    , m_data(varHash)
{
    // If item does not have a guid, fall back to some other identifier
    if (m_guid.isEmpty())
        m_guid = varHash.value(KeyTorrentURL).toString();
    if (m_guid.isEmpty())
        m_guid = varHash.value(KeyTitle).toString();
    if (m_guid.isEmpty())
        throw std::runtime_error("Bad RSS Article data");

    m_data[KeyId] = m_guid;

    if (m_torrentURL.isEmpty()) {
        m_torrentURL = m_link;
        m_data[KeyTorrentURL] = m_torrentURL;
    }
}

Article::Article(Feed *feed, const QJsonObject &jsonObj)
    : Article(feed, jsonObj.toVariantHash())
{
    // JSON object store DateTime as string so we need to convert it
    m_date = QDateTime::fromString(jsonObj.value(KeyDate).toString(), Qt::RFC2822Date);
    m_data[KeyDate] = m_date;
}

QString Article::guid() const
{
    return m_guid;
}

QDateTime Article::date() const
{
    return m_date;
}

QString Article::title() const
{
    return m_title;
}

QString Article::author() const
{
    return m_author;
}

QString Article::description() const
{
    return m_description;
}

QString Article::torrentUrl() const
{
    return (m_torrentURL.isEmpty() ? m_link : m_torrentURL);
}

QString Article::link() const
{
    return m_link;
}

bool Article::isRead() const
{
    return m_isRead;
}

QVariantHash Article::data() const
{
    return m_data;
}

void Article::markAsRead()
{
    if (!m_isRead) {
        m_isRead = true;
        m_data[KeyIsRead] = m_isRead;
        emit read(this);
    }
}

QJsonObject Article::toJsonObject() const
{
    auto jsonObj = QJsonObject::fromVariantHash(m_data);
    // JSON object doesn't support DateTime so we need to convert it
    jsonObj[KeyDate] = m_date.toString(Qt::RFC2822Date);

    return jsonObj;
}

bool Article::articleDateRecentThan(Article *article, const QDateTime &date)
{
    return article->date() > date;
}

Feed *Article::feed() const
{
    return m_feed;
}
