#ifndef NET_GEOIPMANAGER_H
#define NET_GEOIPMANAGER_H

#include <QCache>
#include <QObject>

class QHostAddress;
class QString;

class GeoIPDatabase;

namespace Net
{
    class GeoIPManager : public QObject
    {
        Q_OBJECT

    public:
        static void initInstance();
        static void freeInstance();
        static GeoIPManager *instance();

        QString lookup(const QHostAddress &hostAddr) const;

        static QString CountryName(const QString &countryISOCode);

    private slots:
        void configure();
        void downloadFinished(const QString &url, QByteArray data);
        void downloadFailed(const QString &url, const QString &reason);

    private:
        GeoIPManager();
        ~GeoIPManager();

        void loadDatabase();
        void manageDatabaseUpdate();
        void downloadDatabaseFile();

        bool m_enabled;
        GeoIPDatabase *m_geoIPDatabase;

        static GeoIPManager *m_instance;
    };
}

#endif // NET_GEOIPMANAGER_H
