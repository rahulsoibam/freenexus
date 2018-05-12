#ifndef GEOIPDATABASE_H
#define GEOIPDATABASE_H

#include <QtGlobal>
#include <QCoreApplication>

class QHostAddress;
class QString;
class QByteArray;
class QDateTime;

struct DataFieldDescriptor;

class GeoIPDatabase
{
    Q_DECLARE_TR_FUNCTIONS(GeoIPDatabase)

public:
    static GeoIPDatabase *load(const QString &filename, QString &error);
    static GeoIPDatabase *load(const QByteArray &data, QString &error);

    ~GeoIPDatabase();

    QString type() const;
    quint16 ipVersion() const;
    QDateTime buildEpoch() const;
    QString lookup(const QHostAddress &hostAddr) const;

private:
    GeoIPDatabase(quint32 size);

    bool parseMetadata(const QVariantHash &metadata, QString &error);
    bool loadDB(QString &error) const;
    QVariantHash readMetadata() const;

    QVariant readDataField(quint32 &offset) const;
    bool readDataFieldDescriptor(quint32 &offset, DataFieldDescriptor &out) const;
    void fromBigEndian(uchar *buf, quint32 len) const;
    QVariant readMapValue(quint32 &offset, quint32 count) const;
    QVariant readArrayValue(quint32 &offset, quint32 count) const;

    template<typename T>
    QVariant readPlainValue(quint32 &offset, quint8 len) const
    {
        T value = 0;
        const uchar *const data = m_data + offset;
        const quint32 availSize = m_size - offset;

        if ((len > 0) && (len <= sizeof(T) && (availSize >= len))) {
            // copy input data to last 'len' bytes of 'value'
            uchar *dst = reinterpret_cast<uchar *>(&value) + (sizeof(T) - len);
            memcpy(dst, data, len);
            fromBigEndian(reinterpret_cast<uchar *>(&value), sizeof(T));
            offset += len;
        }

        return QVariant::fromValue(value);
    }

    // Metadata
    quint16 m_ipVersion;
    quint16 m_recordSize;
    quint32 m_nodeCount;
    int m_nodeSize;
    int m_indexSize;
    int m_recordBytes;
    QDateTime m_buildEpoch;
    // Search data
    mutable QHash<quint32, QString> m_countries;
    quint32 m_size;
    uchar *m_data;
};

#endif // GEOIPDATABASE_H
