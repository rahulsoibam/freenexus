#ifndef PEERADDITION_H
#define PEERADDITION_H

#include <QDialog>

#include "base/bittorrent/peerinfo.h"

template <class T> class QList;

namespace Ui
{
    class addPeersDialog;
}

class PeersAdditionDlg : public QDialog
{
    Q_OBJECT

public:
    PeersAdditionDlg(QWidget *parent);
    ~PeersAdditionDlg();

    static QList<BitTorrent::PeerAddress> askForPeers(QWidget *parent);

protected slots:
    void validateInput();

private:
    BitTorrent::PeerAddress parsePeer(QString peer);

    Ui::addPeersDialog *m_ui;
    QList<BitTorrent::PeerAddress> m_peersList;
};

#endif // PEERADDITION_H
