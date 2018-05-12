#ifndef BITTORRENT_PEERINFO_H
#define BITTORRENT_PEERINFO_H

#include <QBitArray>
#include <QCoreApplication>
#include <QHostAddress>

#include <libtorrent/peer_info.hpp>

namespace BitTorrent
{
    class TorrentHandle;

    struct PeerAddress
    {
        QHostAddress ip;
        ushort port;

        PeerAddress();
        PeerAddress(QHostAddress ip, ushort port);
    };

    class PeerInfo
    {
        Q_DECLARE_TR_FUNCTIONS(PeerInfo)

    public:
        PeerInfo(const TorrentHandle *torrent, const libtorrent::peer_info &nativeInfo);

        bool fromDHT() const;
        bool fromPeX() const;
        bool fromLSD() const;

        bool isInteresting() const;
        bool isChocked() const;
        bool isRemoteInterested() const;
        bool isRemoteChocked() const;
        bool isSupportsExtensions() const;
        bool isLocalConnection() const;

        bool isHandshake() const;
        bool isConnecting() const;
        bool isOnParole() const;
        bool isSeed() const;

        bool optimisticUnchoke() const;
        bool isSnubbed() const;
        bool isUploadOnly() const;
        bool isEndgameMode() const;
        bool isHolepunched() const;

        bool useI2PSocket() const;
        bool useUTPSocket() const;
        bool useSSLSocket() const;

        bool isRC4Encrypted() const;
        bool isPlaintextEncrypted() const;

        PeerAddress address() const;
        QString client() const;
        qreal progress() const;
        int payloadUpSpeed() const;
        int payloadDownSpeed() const;
        qlonglong totalUpload() const;
        qlonglong totalDownload() const;
        QBitArray pieces() const;
        QString connectionType() const;
        qreal relevance() const;
        QString flags() const;
        QString flagsDescription() const;
#ifndef DISABLE_COUNTRIES_RESOLUTION
        QString country() const;
#endif
        int downloadingPieceIndex() const;

    private:
        void calcRelevance(const TorrentHandle *torrent);
        void determineFlags();

        libtorrent::peer_info m_nativeInfo;
        qreal m_relevance;
        QString m_flags;
        QString m_flagsDescription;
    };
}

#endif // BITTORRENT_PEERINFO_H
