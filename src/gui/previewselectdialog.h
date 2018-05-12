#ifndef PREVIEWSELECTDIALOG_H
#define PREVIEWSELECTDIALOG_H

#include <QDialog>
#include <QList>

#include "base/bittorrent/torrenthandle.h"
#include "base/settingvalue.h"
#include "ui_previewselectdialog.h"

class QStandardItemModel;

class PreviewListDelegate;

class PreviewSelectDialog : public QDialog, private Ui::preview
{
    Q_OBJECT

public:
    enum PreviewColumn
    {
        NAME,
        SIZE,
        PROGRESS,
        FILE_INDEX,

        NB_COLUMNS
    };

    PreviewSelectDialog(QWidget *parent, BitTorrent::TorrentHandle *const torrent);
    ~PreviewSelectDialog();

signals:
    void readyToPreviewFile(QString) const;

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void previewButtonClicked();

private:
    void loadWindowState();
    void saveWindowState();

    QStandardItemModel *m_previewListModel;
    PreviewListDelegate *m_listDelegate;
    BitTorrent::TorrentHandle *const m_torrent;
    bool m_headerStateInitialized = false;

    // Settings
    CachedSettingValue<QSize> m_storeDialogSize;
    CachedSettingValue<QByteArray> m_storeTreeHeaderState;
};

#endif // PREVIEWSELECTDIALOG_H
