#ifndef TRACKERSADDITION_H
#define TRACKERSADDITION_H

#include <QDialog>

class QString;
class QStringList;

namespace BitTorrent
{
    class TorrentHandle;
}

namespace Ui
{
    class TrackersAdditionDlg;
}

class TrackersAdditionDlg : public QDialog
{
    Q_OBJECT

public:
    TrackersAdditionDlg(QWidget *parent, BitTorrent::TorrentHandle *const torrent);
    ~TrackersAdditionDlg();

    QStringList newTrackers() const;
    static QStringList askForTrackers(QWidget *parent, BitTorrent::TorrentHandle *const torrent);

public slots:
    void on_uTorrentListButton_clicked();
    void parseUTorrentList(const QString &, const QString &path);
    void getTrackerError(const QString &, const QString &error);

private:
    Ui::TrackersAdditionDlg *m_ui;
    BitTorrent::TorrentHandle *const m_torrent;
};

#endif // TRACKERSADDITION_H
