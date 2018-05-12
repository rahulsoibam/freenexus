#ifndef BITTORRENT_TRACKER_H
#define BITTORRENT_TRACKER_H

#include <QHash>
#include <QObject>

#include "base/http/irequesthandler.h"
#include "base/http/responsebuilder.h"
#include "base/http/types.h"

namespace libtorrent
{
    class entry;
}

namespace Http
{
    class Server;
}

namespace BitTorrent
{
    struct Peer
    {
        QString ip;
        QByteArray peerId;
        int port;

        bool operator!=(const Peer &other) const;
        bool operator==(const Peer &other) const;
        QString uid() const;
        libtorrent::entry toEntry(bool noPeerId) const;
    };

    struct TrackerAnnounceRequest
    {
        QByteArray infoHash;
        QString event;
        int numwant;
        Peer peer;
        // Extensions
        bool noPeerId;
    };

    typedef QHash<QString, Peer> PeerList;
    typedef QHash<QByteArray, PeerList> TorrentList;

    /* Basic Bittorrent tracker implementation in Qt */
    /* Following http://wiki.theory.org/BitTorrent_Tracker_Protocol */
    class Tracker : public QObject, public Http::IRequestHandler, private Http::ResponseBuilder
    {
        Q_OBJECT
        Q_DISABLE_COPY(Tracker)

    public:
        explicit Tracker(QObject *parent = nullptr);
        ~Tracker();

        bool start();
        Http::Response processRequest(const Http::Request &request, const Http::Environment &env);

    private:
        void respondToAnnounceRequest();
        void registerPeer(const TrackerAnnounceRequest &annonceReq);
        void unregisterPeer(const TrackerAnnounceRequest &annonceReq);
        void replyWithPeerList(const TrackerAnnounceRequest &annonceReq);

        Http::Server *m_server;
        TorrentList m_torrents;

        Http::Request m_request;
        Http::Environment m_env;
    };
}

#endif // BITTORRENT_TRACKER_H
