#ifndef UTILS_STRING_H
#define UTILS_STRING_H

#include <QString>

class QByteArray;
class QLatin1String;
class TriStateBool;

namespace Utils
{
    namespace String
    {
        QString fromDouble(double n, int precision);

        // Implements constant-time comparison to protect against timing attacks
        // Taken from https://crackstation.net/hashing-security.htm
        bool slowEquals(const QByteArray &a, const QByteArray &b);

        int naturalCompare(const QString &left, const QString &right, const Qt::CaseSensitivity caseSensitivity);
        template <Qt::CaseSensitivity caseSensitivity>
        bool naturalLessThan(const QString &left, const QString &right)
        {
            return (naturalCompare(left, right, caseSensitivity) < 0);
        }

        QString wildcardToRegex(const QString &pattern);

        template <typename T>
        T unquote(const T &str, const QString &quotes = QLatin1String("\""))
        {
            if (str.length() < 2) return str;

            for (auto const quote : quotes) {
                if (str.startsWith(quote) && str.endsWith(quote))
                    return str.mid(1, str.length() - 2);
            }

            return str;
        }

        bool parseBool(const QString &string, const bool defaultValue);
        TriStateBool parseTriStateBool(const QString &string);
    }
}

#endif // UTILS_STRING_H
