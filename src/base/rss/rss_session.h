#pragma once

/*
 * RSS Session configuration file format (JSON):
 *
 * =============== BEGIN ===============
 * {
 *     "folder1": {
 *         "subfolder1": {
 *             "Feed name (Alias)": "http://some-feed-url1",
 *             "http://some-feed-url2": ""
 *         },
 *         "subfolder2": {},
 *         "http://some-feed-url3": "",
 *         "Feed name (Alias)": {
 *             "url": "http://some-feed-url4",
 *         }
 *     },
 *     "folder2": {},
 *     "folder3": {}
 * }
 * ================ END ================
 *
 * 1.   Document is JSON object (the same as Folder)
 * 2.   Folder is JSON object (keys are Item names, values are Items)
 * 3.1. Feed is JSON object (keys are property names, values are property values; 'url' is required)
 * 3.2. (Reduced format) Feed is JSON string (string is URL unless it's empty, otherwise we take Feed URL from name)
 */

#include <QHash>
#include <QObject>
#include <QPointer>
#include <QStringList>
#include <QTimer>

class QThread;
class Application;
class AsyncFileStorage;

namespace RSS
{
    class Item;
    class Feed;
    class Folder;

    class Session: public QObject
    {
        Q_OBJECT
        Q_DISABLE_COPY(Session)

        friend class ::Application;

        Session();
        ~Session() override;

    public:
        static Session *instance();

        bool isProcessingEnabled() const;
        void setProcessingEnabled(bool enabled);

        QThread *workingThread() const;
        AsyncFileStorage *confFileStorage() const;
        AsyncFileStorage *dataFileStorage() const;

        int maxArticlesPerFeed() const;
        void setMaxArticlesPerFeed(int n);

        uint refreshInterval() const;
        void setRefreshInterval(uint refreshInterval);

        bool addFolder(const QString &path, QString *error = nullptr);
        bool addFeed(const QString &url, const QString &path, QString *error = nullptr);
        bool moveItem(const QString &itemPath, const QString &destPath
                         , QString *error = nullptr);
        bool moveItem(Item *item, const QString &destPath, QString *error = nullptr);
        bool removeItem(const QString &itemPath, QString *error = nullptr);

        QList<Item *> items() const;
        Item *itemByPath(const QString &path) const;
        QList<Feed *> feeds() const;
        Feed *feedByURL(const QString &url) const;

        Folder *rootFolder() const;

    public slots:
        void refresh();

    signals:
        void processingStateChanged(bool enabled);
        void maxArticlesPerFeedChanged(int n);
        void itemAdded(Item *item);
        void itemPathChanged(Item *item);
        void itemAboutToBeRemoved(Item *item);
        void feedIconLoaded(Feed *feed);
        void feedStateChanged(Feed *feed);

    private slots:
        void handleItemAboutToBeDestroyed(Item *item);
        void handleFeedTitleChanged(Feed *feed);

    private:
        void load();
        void loadFolder(const QJsonObject &jsonObj, Folder *folder);
        void loadLegacy();
        void store();
        Folder *prepareItemDest(const QString &path, QString *error);
        Folder *addSubfolder(const QString &name, Folder *parentFolder);
        Feed *addFeedToFolder(const QString &url, const QString &name, Folder *parentFolder);
        void addItem(Item *item, Folder *destFolder);

        static QPointer<Session> m_instance;

        bool m_processingEnabled;
        QThread *m_workingThread;
        AsyncFileStorage *m_confFileStorage;
        AsyncFileStorage *m_dataFileStorage;
        QTimer m_refreshTimer;
        uint m_refreshInterval;
        int m_maxArticlesPerFeed;
        QHash<QString, Item *> m_itemsByPath;
        QHash<QString, Feed *> m_feedsByURL;
    };
}
