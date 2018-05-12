#ifndef TRACKERLOGIN_H
#define TRACKERLOGIN_H

#include <QDialog>

#include "ui_login.h"

namespace BitTorrent
{
    class TorrentHandle;
}

class trackerLogin : public QDialog, private Ui::authentication{
  Q_OBJECT

  private:
    BitTorrent::TorrentHandle *const m_torrent;

  public:
    trackerLogin(QWidget *parent, BitTorrent::TorrentHandle *const torrent);
    ~trackerLogin();

  signals:
    void trackerLoginCancelled(QPair<BitTorrent::TorrentHandle*, QString> tracker);

  private slots:
    void loginButtonClicked();
    void cancelButtonClicked();
};

#endif
