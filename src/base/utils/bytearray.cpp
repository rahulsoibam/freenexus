#include "bytearray.h"
#include <QList>

QList<QByteArray> Utils::ByteArray::splitToViews(const QByteArray &in, const QByteArray &sep, const QString::SplitBehavior behavior)
{
    if (sep.isEmpty())
        return {in};

    QList<QByteArray> ret;

    int head = 0;
    while (head < in.size()) {
        int end = in.indexOf(sep, head);
        if (end < 0)
            end = in.size();

        // omit empty parts
        const QByteArray part = QByteArray::fromRawData((in.constData() + head), (end - head));
        if (!part.isEmpty() || (behavior == QString::KeepEmptyParts))
            ret += part;

        head = end + sep.size();
    }

    return ret;
}

const QByteArray Utils::ByteArray::midView(const QByteArray &in, const int pos, const int len)
{
    if ((pos < 0) || (pos >= in.size()) || (len == 0))
        return {};

    const int validLen = ((len < 0) || (pos + len) >= in.size())
            ? in.size() - pos
            : len;
    return QByteArray::fromRawData(in.constData() + pos, validLen);
}
