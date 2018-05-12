#ifndef BASE_UTILS_NET_H
#define BASE_UTILS_NET_H

#include <QList>
#include <QPair>

class QHostAddress;
class QString;
class QStringList;

namespace Utils
{
    namespace Net
    {
        using Subnet = QPair<QHostAddress, int>;

        bool isValidIP(const QString &ip);
        Subnet parseSubnet(const QString &subnetStr, bool *ok = nullptr);
        bool canParseSubnet(const QString &subnetStr);
        bool isLoopbackAddress(const QHostAddress &addr);
        bool isIPInRange(const QHostAddress &addr, const QList<Subnet> &subnets);
        QString subnetToString(const Subnet &subnet);
    }
}

#endif // BASE_UTILS_NET_H
