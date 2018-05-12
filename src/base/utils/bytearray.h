#pragma once

#include <QByteArray>
#include <QString>

namespace Utils
{
    namespace ByteArray
    {
        // Mimic QString::split(sep, behavior)
        QList<QByteArray> splitToViews(const QByteArray &in, const QByteArray &sep, const QString::SplitBehavior behavior = QString::KeepEmptyParts);

        // Mimic QByteArray::mid(pos, len) but instead of returning a full-copy,
        // we only return a partial view
        const QByteArray midView(const QByteArray &in, const int pos, const int len = -1);
    }
}
