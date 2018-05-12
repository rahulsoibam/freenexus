#pragma once

#include <QDateTime>
#include <QObject>
#include <QString>
#include <QVariantHash>

namespace RSS
{
    class Feed;

    class Article: public QObject
    {
        Q_OBJECT
        Q_DISABLE_COPY(Article)

        friend class Feed;

        Article(Feed *feed, const QVariantHash &varHash);
        Article(Feed *feed, const QJsonObject &jsonObj);

    public:
        static const QString KeyId;
        static const QString KeyDate;
        static const QString KeyTitle;
        static const QString KeyAuthor;
        static const QString KeyDescription;
        static const QString KeyTorrentURL;
        static const QString KeyLink;
        static const QString KeyIsRead;

        Feed *feed() const;
        QString guid() const;
        QDateTime date() const;
        QString title() const;
        QString author() const;
        QString description() const;
        QString torrentUrl() const;
        QString link() const;
        bool isRead() const;
        QVariantHash data() const;

        void markAsRead();

        QJsonObject toJsonObject() const;

        static bool articleDateRecentThan(Article *article, const QDateTime &date);

    signals:
        void read(Article *article = nullptr);

    private:
        Feed *m_feed = nullptr;
        QString m_guid;
        QDateTime m_date;
        QString m_title;
        QString m_author;
        QString m_description;
        QString m_torrentURL;
        QString m_link;
        bool m_isRead = false;
        QVariantHash m_data;
    };
}
