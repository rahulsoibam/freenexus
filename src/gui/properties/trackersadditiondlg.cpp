#include "trackersadditiondlg.h"

#include <QFile>
#include <QMessageBox>
#include <QStringList>
#include <QUrl>

#include "base/bittorrent/torrenthandle.h"
#include "base/bittorrent/trackerentry.h"
#include "base/net/downloadhandler.h"
#include "base/net/downloadmanager.h"
#include "base/utils/fs.h"
#include "base/utils/misc.h"
#include "guiiconprovider.h"
#include "ui_trackersadditiondlg.h"

TrackersAdditionDlg::TrackersAdditionDlg(QWidget *parent, BitTorrent::TorrentHandle *const torrent)
    : QDialog(parent)
    , m_ui(new Ui::TrackersAdditionDlg())
    , m_torrent(torrent)
{
    m_ui->setupUi(this);
    // Icons
    m_ui->uTorrentListButton->setIcon(GuiIconProvider::instance()->getIcon("download"));
}

TrackersAdditionDlg::~TrackersAdditionDlg()
{
    delete m_ui;
}

QStringList TrackersAdditionDlg::newTrackers() const
{
    QStringList cleanTrackers;
    foreach (QString url, m_ui->trackers_list->toPlainText().split("\n")) {
        url = url.trimmed();
        if (!url.isEmpty())
            cleanTrackers << url;
    }
    return cleanTrackers;
}

void TrackersAdditionDlg::on_uTorrentListButton_clicked()
{
    m_ui->uTorrentListButton->setEnabled(false);
    Net::DownloadHandler *handler = Net::DownloadManager::instance()->downloadUrl(m_ui->list_url->text(), true);
    connect(handler, SIGNAL(downloadFinished(QString, QString)), this, SLOT(parseUTorrentList(QString, QString)));
    connect(handler, SIGNAL(downloadFailed(QString, QString)), this, SLOT(getTrackerError(QString, QString)));
    // Just to show that it takes times
    setCursor(Qt::WaitCursor);
}

void TrackersAdditionDlg::parseUTorrentList(const QString &, const QString &path)
{
    QFile listFile(path);
    if (!listFile.open(QFile::ReadOnly)) {
        QMessageBox::warning(this, tr("I/O Error"), tr("Error while trying to open the downloaded file."), QMessageBox::Ok);
        setCursor(Qt::ArrowCursor);
        m_ui->uTorrentListButton->setEnabled(true);
        Utils::Fs::forceRemove(path);
        return;
    }

    // Load from torrent handle
    QList<BitTorrent::TrackerEntry> existingTrackers = m_torrent->trackers();
    // Load from current user list
    QStringList tmp = m_ui->trackers_list->toPlainText().split("\n");
    foreach (const QString &userURL, tmp) {
        BitTorrent::TrackerEntry userTracker(userURL);
        if (!existingTrackers.contains(userTracker))
            existingTrackers << userTracker;
    }

    // Add new trackers to the list
    if (!m_ui->trackers_list->toPlainText().isEmpty() && !m_ui->trackers_list->toPlainText().endsWith("\n"))
        m_ui->trackers_list->insertPlainText("\n");
    int nb = 0;
    while (!listFile.atEnd()) {
        const QString line = listFile.readLine().trimmed();
        if (line.isEmpty()) continue;
        BitTorrent::TrackerEntry newTracker(line);
        if (!existingTrackers.contains(newTracker)) {
            m_ui->trackers_list->insertPlainText(line + "\n");
            ++nb;
        }
    }
    // Clean up
    listFile.close();
    Utils::Fs::forceRemove(path);
    //To restore the cursor ...
    setCursor(Qt::ArrowCursor);
    m_ui->uTorrentListButton->setEnabled(true);
    // Display information message if necessary
    if (nb == 0)
        QMessageBox::information(this, tr("No change"), tr("No additional trackers were found."), QMessageBox::Ok);
}

void TrackersAdditionDlg::getTrackerError(const QString &, const QString &error)
{
    //To restore the cursor ...
    setCursor(Qt::ArrowCursor);
    m_ui->uTorrentListButton->setEnabled(true);
    QMessageBox::warning(this, tr("Download error"), tr("The trackers list could not be downloaded, reason: %1").arg(error), QMessageBox::Ok);
}

QStringList TrackersAdditionDlg::askForTrackers(QWidget *parent, BitTorrent::TorrentHandle *const torrent)
{
    QStringList trackers;
    TrackersAdditionDlg dlg(parent, torrent);
    if (dlg.exec() == QDialog::Accepted)
        return dlg.newTrackers();

    return trackers;
}
