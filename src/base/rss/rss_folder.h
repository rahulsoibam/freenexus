#pragma once

#include <QList>
#include "rss_item.h"

namespace RSS
{
    class Session;

    class Folder final: public Item
    {
        Q_OBJECT
        Q_DISABLE_COPY(Folder)

        friend class Session;

        explicit Folder(const QString &path = "");
        ~Folder() override;

    public:
        QList<Article *> articles() const override;
        int unreadCount() const override;
        void markAsRead() override;
        void refresh() override;

        QList<Item *> items() const;

        QJsonValue toJsonValue(bool withData = false) const override;

    private slots:
        void handleItemUnreadCountChanged();

    private:
        void cleanup() override;
        void addItem(Item *item);
        void removeItem(Item *item);

        QList<Item *> m_items;
    };
}
