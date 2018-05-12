#pragma once

#include <QList>
#include <QObject>

namespace RSS
{
    class Article;
    class Folder;
    class Session;

    class Item: public QObject
    {
        Q_OBJECT
        Q_DISABLE_COPY(Item)

        friend class Folder;
        friend class Session;

    public:
        virtual QList<Article *> articles() const = 0;
        virtual int unreadCount() const = 0;
        virtual void markAsRead() = 0;
        virtual void refresh() = 0;

        QString path() const;
        QString name() const;

        virtual QJsonValue toJsonValue(bool withData = false) const = 0;

        static const QString PathSeparator;

        static bool isValidPath(const QString &path);
        static QString joinPath(const QString &path1, const QString &path2);
        static QStringList expandPath(const QString &path);
        static QString parentPath(const QString &path);
        static QString relativeName(const QString &path);

    signals:
        void pathChanged(Item *item = nullptr);
        void unreadCountChanged(Item *item = nullptr);
        void aboutToBeDestroyed(Item *item = nullptr);
        void newArticle(Article *article);
        void articleRead(Article *article);
        void articleAboutToBeRemoved(Article *article);

    protected:
        explicit Item(const QString &path);
        ~Item() override;

        virtual void cleanup() = 0;

    private:
        void setPath(const QString &path);

        QString m_path;
    };
}
