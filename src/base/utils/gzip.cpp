#include "gzip.h"

#include <vector>

#include <QByteArray>

#ifndef ZLIB_CONST
#define ZLIB_CONST  // make z_stream.next_in const
#endif
#include <zlib.h>

QByteArray Utils::Gzip::compress(const QByteArray &data, const int level, bool *ok)
{
    if (ok) *ok = false;

    if (data.isEmpty())
        return {};

    const int BUFSIZE = 128 * 1024;
    std::vector<char> tmpBuf(BUFSIZE);

    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.next_in = reinterpret_cast<const Bytef *>(data.constData());
    strm.avail_in = uInt(data.size());
    strm.next_out = reinterpret_cast<Bytef *>(tmpBuf.data());
    strm.avail_out = BUFSIZE;

    // windowBits = 15 + 16 to enable gzip
    // From the zlib manual: windowBits can also be greater than 15 for optional gzip encoding. Add 16 to windowBits
    // to write a simple gzip header and trailer around the compressed data instead of a zlib wrapper.
    int result = deflateInit2(&strm, level, Z_DEFLATED, (15 + 16), 9, Z_DEFAULT_STRATEGY);
    if (result != Z_OK)
        return {};

    QByteArray output;
    output.reserve(deflateBound(&strm, data.size()));

    // feed to deflate
    while (strm.avail_in > 0) {
        result = deflate(&strm, Z_NO_FLUSH);

        if (result != Z_OK) {
            deflateEnd(&strm);
            return {};
        }

        output.append(tmpBuf.data(), (BUFSIZE - strm.avail_out));
        strm.next_out = reinterpret_cast<Bytef *>(tmpBuf.data());
        strm.avail_out = BUFSIZE;
    }

    // flush the rest from deflate
    while (result != Z_STREAM_END) {
        result = deflate(&strm, Z_FINISH);

        output.append(tmpBuf.data(), (BUFSIZE - strm.avail_out));
        strm.next_out = reinterpret_cast<Bytef *>(tmpBuf.data());
        strm.avail_out = BUFSIZE;
    }

    deflateEnd(&strm);

    if (ok) *ok = true;
    return output;
}

QByteArray Utils::Gzip::decompress(const QByteArray &data, bool *ok)
{
    if (ok) *ok = false;

    if (data.isEmpty())
        return {};

    const int BUFSIZE = 1024 * 1024;
    std::vector<char> tmpBuf(BUFSIZE);

    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.next_in = reinterpret_cast<const Bytef *>(data.constData());
    strm.avail_in = uInt(data.size());
    strm.next_out = reinterpret_cast<Bytef *>(tmpBuf.data());
    strm.avail_out = BUFSIZE;

    // windowBits must be greater than or equal to the windowBits value provided to deflateInit2() while compressing
    // Add 32 to windowBits to enable zlib and gzip decoding with automatic header detection
    int result = inflateInit2(&strm, (15 + 32));
    if (result != Z_OK)
        return {};

    QByteArray output;
    // from lzbench, level 9 average compression ratio is: 31.92%, which decompression ratio is: 1 / 0.3192 = 3.13
    output.reserve(data.size() * 3);

    // run inflate
    while (true) {
        result = inflate(&strm, Z_NO_FLUSH);

        if (result == Z_STREAM_END) {
            output.append(tmpBuf.data(), (BUFSIZE - strm.avail_out));
            break;
        }

        if (result != Z_OK) {
            inflateEnd(&strm);
            return {};
        }

        output.append(tmpBuf.data(), (BUFSIZE - strm.avail_out));
        strm.next_out = reinterpret_cast<Bytef *>(tmpBuf.data());
        strm.avail_out = BUFSIZE;
    }

    inflateEnd(&strm);

    if (ok) *ok = true;
    return output;
}
