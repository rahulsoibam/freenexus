#include "net.h"
#include <QHostAddress>
#include <QString>
#include <QStringList>

namespace Utils
{
    namespace Net
    {
        bool isValidIP(const QString &ip)
        {
            return !QHostAddress(ip).isNull();
        }

        Subnet parseSubnet(const QString &subnetStr, bool *ok)
        {
            const Subnet invalid = qMakePair(QHostAddress(), -1);
            const Subnet subnet = QHostAddress::parseSubnet(subnetStr);
            if (ok)
                *ok = (subnet != invalid);
            return subnet;
        }

        bool canParseSubnet(const QString &subnetStr)
        {
            bool ok = false;
            parseSubnet(subnetStr, &ok);
            return ok;
        }

        bool isLoopbackAddress(const QHostAddress &addr)
        {
            return (addr == QHostAddress::LocalHost)
                    || (addr == QHostAddress::LocalHostIPv6)
                    || (addr == QHostAddress(QLatin1String("::ffff:127.0.0.1")));
        }

        bool isIPInRange(const QHostAddress &addr, const QList<Subnet> &subnets)
        {
            QHostAddress protocolEquivalentAddress;
            bool addrConversionOk = false;

            if (addr.protocol() == QAbstractSocket::IPv4Protocol) {
                // always succeeds
                protocolEquivalentAddress = QHostAddress(addr.toIPv6Address());
                addrConversionOk = true;
            }
            else {
                // only succeeds when addr is an ipv4-mapped ipv6 address
                protocolEquivalentAddress = QHostAddress(addr.toIPv4Address(&addrConversionOk));
            }

            for (const Subnet &subnet : subnets)
                if (addr.isInSubnet(subnet) || (addrConversionOk && protocolEquivalentAddress.isInSubnet(subnet)))
                    return true;

            return false;
        }

        QString subnetToString(const Subnet &subnet)
        {
            return subnet.first.toString() + '/' + QString::number(subnet.second);
        }
    }
}
