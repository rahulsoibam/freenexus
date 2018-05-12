#ifndef PEERLISTWIDGET_H
#define PEERLISTWIDGET_H

#include <QHash>
#include <QPointer>
#include <QSet>
#include <QShortcut>
#include <QTreeView>

namespace Net
{
    class ReverseResolution;
}

class PeerListDelegate;
class PeerListSortModel;
class PropertiesWidget;

class QSortFilterProxyModel;
class QStandardItem;
class QStandardItemModel;

namespace BitTorrent
{
    class TorrentHandle;
    class PeerInfo;
    struct PeerAddress;
}

class PeerListWidget : public QTreeView
{
    Q_OBJECT

public:
    explicit PeerListWidget(PropertiesWidget *parent);
    ~PeerListWidget();

    void loadPeers(BitTorrent::TorrentHandle *const torrent, bool forceHostnameResolution = false);
    QStandardItem *addPeer(const QString &ip, BitTorrent::TorrentHandle *const torrent, const BitTorrent::PeerInfo &peer);
    void updatePeer(const QString &ip, BitTorrent::TorrentHandle *const torrent, const BitTorrent::PeerInfo &peer);
    void updatePeerHostNameResolutionState();
    void updatePeerCountryResolutionState();
    void clear();

private slots:
    void loadSettings();
    void saveSettings() const;
    void displayToggleColumnsMenu(const QPoint &);
    void showPeerListMenu(const QPoint &);
    void banSelectedPeers();
    void copySelectedPeers();
    void handleSortColumnChanged(int col);
    void handleResolved(const QString &ip, const QString &hostname);

private:
    void wheelEvent(QWheelEvent *event) override;

    QStandardItemModel *m_listModel;
    PeerListDelegate *m_listDelegate;
    PeerListSortModel *m_proxyModel;
    QHash<QString, QStandardItem *> m_peerItems;
    QHash<QString, BitTorrent::PeerAddress> m_peerAddresses;
    QSet<QString> m_missingFlags;
    QPointer<Net::ReverseResolution> m_resolver;
    PropertiesWidget *m_properties;
    bool m_resolveCountries;
    QShortcut *m_copyHotkey;
};

#endif // PEERLISTWIDGET_H
