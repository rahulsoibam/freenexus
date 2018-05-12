#pragma once

#include <QBasicTimer>
#include <QHash>
#include <QList>

#include "rss_item.h"

class AsyncFileStorage;

namespace RSS
{
    class Article;
    class Session;

    namespace Private
    {
        class Parser;
        struct ParsingResult;
    }

    class Feed final: public Item
    {
        Q_OBJECT
        Q_DISABLE_COPY(Feed)

        friend class Session;

        Feed(const QString &url, const QString &path, Session *session);
        ~Feed() override;

    public:
        QList<Article *> articles() const override;
        int unreadCount() const override;
        void markAsRead() override;
        void refresh() override;

        QString url() const;
        QString title() const;
        QString lastBuildDate() const;
        bool hasError() const;
        bool isLoading() const;
        Article *articleByGUID(const QString &guid) const;
        QString iconPath() const;

        QJsonValue toJsonValue(bool withData = false) const override;

    signals:
        void iconLoaded(Feed *feed = nullptr);
        void titleChanged(Feed *feed = nullptr);
        void stateChanged(Feed *feed = nullptr);

    private slots:
        void handleSessionProcessingEnabledChanged(bool enabled);
        void handleMaxArticlesPerFeedChanged(int n);
        void handleIconDownloadFinished(const QString &url, const QString &filePath);
        void handleDownloadFinished(const QString &url, const QByteArray &data);
        void handleDownloadFailed(const QString &url, const QString &error);
        void handleParsingFinished(const Private::ParsingResult &result);
        void handleArticleRead(Article *article);

    private:
        void timerEvent(QTimerEvent *event) override;
        void cleanup() override;
        void load();
        void loadArticles(const QByteArray &data);
        void loadArticlesLegacy();
        void store();
        void storeDeferred();
        bool addArticle(Article *article);
        void removeOldestArticle();
        void increaseUnreadCount();
        void decreaseUnreadCount();
        void downloadIcon();

        Session *m_session;
        Private::Parser *m_parser;
        const QString m_url;
        QString m_title;
        QString m_lastBuildDate;
        bool m_hasError = false;
        bool m_isLoading = false;
        QHash<QString, Article *> m_articles;
        QList<Article *> m_articlesByDate;
        int m_unreadCount = 0;
        QString m_iconPath;
        QString m_dataFileName;
        QBasicTimer m_savingTimer;
        bool m_dirty = false;
    };
}
