#ifndef BITTORRENT_SESSIONSTATUS_H
#define BITTORRENT_SESSIONSTATUS_H

#include <QtGlobal>

namespace BitTorrent
{
    struct SessionStatus
    {
        bool hasIncomingConnections = false;

        // Current download rate for the BT
        // session. Payload means that it only take into
        // account "useful" part of the rate
        quint64 payloadDownloadRate = 0;

        // Current upload rate for the BT
        // session. Payload means that it only take into
        // account "useful" part of the rate
        quint64 payloadUploadRate = 0;

        // Additional download/upload rates
        quint64 uploadRate = 0;
        quint64 downloadRate = 0;
        quint64 ipOverheadUploadRate = 0;
        quint64 ipOverheadDownloadRate = 0;
        quint64 dhtUploadRate = 0;
        quint64 dhtDownloadRate = 0;
        quint64 trackerUploadRate = 0;
        quint64 trackerDownloadRate = 0;

        quint64 totalDownload = 0;
        quint64 totalUpload = 0;
        quint64 totalPayloadDownload = 0;
        quint64 totalPayloadUpload = 0;
        quint64 ipOverheadUpload = 0;
        quint64 ipOverheadDownload = 0;
        quint64 dhtUpload = 0;
        quint64 dhtDownload = 0;
        quint64 trackerUpload = 0;
        quint64 trackerDownload = 0;
        quint64 totalWasted = 0;
        quint64 diskReadQueue = 0;
        quint64 diskWriteQueue = 0;
        quint64 dhtNodes = 0;
        quint64 peersCount = 0;
    };
}

#endif // BITTORRENT_SESSIONSTATUS_H
