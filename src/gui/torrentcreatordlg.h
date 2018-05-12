#ifndef TORRENTCREATORDLG_H
#define TORRENTCREATORDLG_H

#include <QDialog>

#include "base/settingvalue.h"

namespace Ui
{
    class TorrentCreatorDlg;
}

namespace BitTorrent
{
    class TorrentCreatorThread;
}

class TorrentCreatorDlg: public QDialog
{
    Q_OBJECT

public:
    TorrentCreatorDlg(QWidget *parent = 0, const QString &defaultPath = QString());
    ~TorrentCreatorDlg();
    void updateInputPath(const QString &path);

private slots:
    void updateProgressBar(int progress);
    void updatePiecesCount();
    void onCreateButtonClicked();
    void onAddFileButtonClicked();
    void onAddFolderButtonClicked();
    void handleCreationFailure(const QString &msg);
    void handleCreationSuccess(const QString &path, const QString &branchPath);

private:
    void dropEvent(QDropEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;

    void saveSettings();
    void loadSettings();
    int getPieceSize() const;
    void setInteractionEnabled(bool enabled);

    Ui::TorrentCreatorDlg *m_ui;
    BitTorrent::TorrentCreatorThread *m_creatorThread;

    // settings
    CachedSettingValue<QSize> m_storeDialogSize;
    CachedSettingValue<int> m_storePieceSize;
    CachedSettingValue<bool> m_storePrivateTorrent;
    CachedSettingValue<bool> m_storeStartSeeding;
    CachedSettingValue<bool> m_storeIgnoreRatio;
    CachedSettingValue<bool> m_storeOptimizeAlignment;
    CachedSettingValue<QString> m_storeLastAddPath;
    CachedSettingValue<QString> m_storeTrackerList;
    CachedSettingValue<QString> m_storeWebSeedList;
    CachedSettingValue<QString> m_storeComments;
    CachedSettingValue<QString> m_storeLastSavePath;
    CachedSettingValue<QString> m_storeSource;
};

#endif
