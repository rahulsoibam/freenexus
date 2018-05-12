#ifndef UTILS_GZIP_H
#define UTILS_GZIP_H

class QByteArray;

namespace Utils
{
    namespace Gzip
    {
        QByteArray compress(const QByteArray &data, int level = 6, bool *ok = nullptr);
        QByteArray decompress(const QByteArray &data, bool *ok = nullptr);
    }
}

#endif // UTILS_GZIP_H
