#include "peersadditiondlg.h"

#include <QHostAddress>
#include <QMessageBox>

#include "ui_peersadditiondlg.h"

PeersAdditionDlg::PeersAdditionDlg(QWidget *parent)
    : QDialog(parent)
    , m_ui(new Ui::addPeersDialog())
{
    m_ui->setupUi(this);
    connect(m_ui->buttonBox, SIGNAL(accepted()), this, SLOT(validateInput()));

    m_ui->label_format->hide();
    m_ui->peers_txt->setPlaceholderText("Format: IPv4:port / [IPv6]:port");
}

PeersAdditionDlg::~PeersAdditionDlg()
{
    delete m_ui;
}

QList<BitTorrent::PeerAddress> PeersAdditionDlg::askForPeers(QWidget *parent)
{
    PeersAdditionDlg dlg(parent);
    dlg.exec();
    return dlg.m_peersList;
}

void PeersAdditionDlg::validateInput()
{
    if (m_ui->peers_txt->toPlainText().trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("No peer entered"),
                    tr("Please type at least one peer."),
                    QMessageBox::Ok);
        return;
    }
    foreach (const QString &peer, m_ui->peers_txt->toPlainText().trimmed().split("\n")) {
        BitTorrent::PeerAddress addr = parsePeer(peer);
        if (!addr.ip.isNull()) {
            m_peersList.append(addr);
        }
        else {
            QMessageBox::warning(this, tr("Invalid peer"),
                    tr("The peer '%1' is invalid.").arg(peer),
                    QMessageBox::Ok);
            m_peersList.clear();
            return;
        }
    }
    accept();
}

BitTorrent::PeerAddress PeersAdditionDlg::parsePeer(QString peer)
{
    BitTorrent::PeerAddress addr;
    QStringList ipPort;

    if (peer[0] == '[' && peer.indexOf("]:") != -1) // IPv6
        ipPort = peer.remove(QChar('[')).split("]:");
    else if (peer.indexOf(":") != -1) // IPv4
        ipPort = peer.split(":");
    else
        return addr;

    QHostAddress ip(ipPort[0]);
    if (ip.isNull())
        return addr;

    bool ok;
    int port = ipPort[1].toInt(&ok);
    if (!ok || port < 1 || port > 65535)
        return addr;

    addr.ip = ip;
    addr.port = port;
    return addr;
}
