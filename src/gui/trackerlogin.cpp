#include "trackerlogin.h"

#include <libtorrent/version.hpp>

#include <QPushButton>

#include "base/bittorrent/torrenthandle.h"
#include "utils.h"

trackerLogin::trackerLogin(QWidget *parent, BitTorrent::TorrentHandle *const torrent)
  : QDialog(parent)
  , m_torrent(torrent)
{
  setupUi(this);
  setAttribute(Qt::WA_DeleteOnClose);

  buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Log in"));

  login_logo->setPixmap(QPixmap(QLatin1String(":/icons/qbt-theme/encrypted.png")));

  tracker_url->setText(torrent->currentTracker());

  connect(buttonBox, &QDialogButtonBox::accepted, this, &trackerLogin::loginButtonClicked);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &trackerLogin::cancelButtonClicked);
  connect(linePasswd, &QLineEdit::returnPressed, this, &trackerLogin::loginButtonClicked);
  connect(this, SIGNAL(trackerLoginCancelled(QPair<BitTorrent::TorrentHandle*, QString>)),  // TODO: use Qt5 connect syntax
    parent, SLOT(addUnauthenticatedTracker(QPair<BitTorrent::TorrentHandle*, QString>)));

  Utils::Gui::resize(this);
  show();
}

trackerLogin::~trackerLogin() {}

void trackerLogin::loginButtonClicked()
{
  // login
#if LIBTORRENT_VERSION_NUM < 10100
  m_torrent->setTrackerLogin(lineUsername->text(), linePasswd->text());
#endif
  accept();
}

void trackerLogin::cancelButtonClicked()
{
  // Emit a signal to GUI to stop asking for authentication
  emit trackerLoginCancelled(qMakePair(m_torrent, m_torrent->currentTracker()));
  reject();
}
