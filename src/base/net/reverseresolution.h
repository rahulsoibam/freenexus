#ifndef NET_REVERSERESOLUTION_H
#define NET_REVERSERESOLUTION_H

#include <QCache>
#include <QObject>

QT_BEGIN_NAMESPACE
class QHostInfo;
class QString;
QT_END_NAMESPACE

namespace Net
{
    class ReverseResolution : public QObject
    {
        Q_OBJECT
        Q_DISABLE_COPY(ReverseResolution)

    public:
        explicit ReverseResolution(QObject *parent = 0);
        ~ReverseResolution();

        void resolve(const QString &ip);

    signals:
        void ipResolved(const QString &ip, const QString &hostname);

    private slots:
        void hostResolved(const QHostInfo &host);

    private:
        QHash<int /* LookupID */, QString /* IP */> m_lookups;
        QCache<QString /* IP */, QString /* HostName */> m_cache;
    };
}

#endif // NET_REVERSERESOLUTION_H
