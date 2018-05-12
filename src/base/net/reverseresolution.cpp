#include <QDebug>
#include <QHostInfo>
#include <QString>

#include <boost/version.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "reverseresolution.h"

const int CACHE_SIZE = 500;

using namespace Net;

static inline bool isUsefulHostName(const QString &hostname, const QString &ip)
{
    return (!hostname.isEmpty() && hostname != ip);
}

ReverseResolution::ReverseResolution(QObject *parent)
    : QObject(parent)
{
    m_cache.setMaxCost(CACHE_SIZE);
}

ReverseResolution::~ReverseResolution()
{
    qDebug("Deleting host name resolver...");
}

void ReverseResolution::resolve(const QString &ip)
{
    if (m_cache.contains(ip)) {
        const QString &hostname = *m_cache.object(ip);
        qDebug() << "Resolved host name using cache: " << ip << " -> " << hostname;
        if (isUsefulHostName(hostname, ip))
            emit ipResolved(ip, hostname);
    }
    else {
        // Actually resolve the ip
        m_lookups.insert(QHostInfo::lookupHost(ip, this, SLOT(hostResolved(QHostInfo))), ip);
    }
}

void ReverseResolution::hostResolved(const QHostInfo &host)
{
    const QString &ip = m_lookups.take(host.lookupId());
    Q_ASSERT(!ip.isNull());

    if (host.error() != QHostInfo::NoError) {
        qDebug() << "DNS Reverse resolution error: " << host.errorString();
        return;
    }

    const QString &hostname = host.hostName();

    qDebug() << Q_FUNC_INFO << ip << QString("->") << hostname;
    m_cache.insert(ip, new QString(hostname));
    if (isUsefulHostName(hostname, ip))
        emit ipResolved(ip, hostname);
}
