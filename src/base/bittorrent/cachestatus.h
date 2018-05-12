#ifndef BITTORRENT_CACHESTATUS_H
#define BITTORRENT_CACHESTATUS_H

#include <QtGlobal>

namespace BitTorrent
{
    struct CacheStatus
    {
        quint64 totalUsedBuffers = 0;
        quint64 jobQueueLength = 0;
        quint64 averageJobTime = 0;
        quint64 queuedBytes = 0;
        qreal readRatio = 0.0;
    };
}

#endif // BITTORRENT_CACHESTATUS_H
