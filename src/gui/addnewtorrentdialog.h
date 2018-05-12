#ifndef ADDNEWTORRENTDIALOG_H
#define ADDNEWTORRENTDIALOG_H

#include <QDialog>
#include <QScopedPointer>
#include <QShortcut>
#include <QUrl>

#include "base/bittorrent/infohash.h"
#include "base/bittorrent/torrentinfo.h"
#include "base/bittorrent/addtorrentparams.h"

namespace BitTorrent
{
    class MagnetUri;
}

namespace Ui
{
    class AddNewTorrentDialog;
}

class TorrentContentFilterModel;
class TorrentFileGuard;
class PropListDelegate;
template <typename T> class CachedSettingValue;

class AddNewTorrentDialog: public QDialog
{
    Q_OBJECT

public:
    static constexpr int minPathHistoryLength = 0;
    static constexpr int maxPathHistoryLength = 99;

    ~AddNewTorrentDialog();

    static bool isEnabled();
    static void setEnabled(bool value);
    static bool isTopLevel();
    static void setTopLevel(bool value);
    static int savePathHistoryLength();
    static void setSavePathHistoryLength(int value);

    static void show(QString source, const BitTorrent::AddTorrentParams &inParams, QWidget *parent);
    static void show(QString source, QWidget *parent);

private slots:
    void showAdvancedSettings(bool show);
    void displayContentTreeMenu(const QPoint &);
    void updateDiskSpaceLabel();
    void onSavePathChanged(const QString &newPath);
    void renameSelectedFile();
    void updateMetadata(const BitTorrent::TorrentInfo &info);
    void handleDownloadFailed(const QString &url, const QString &reason);
    void handleRedirectedToMagnet(const QString &url, const QString &magnetUri);
    void handleDownloadFinished(const QString &url, const QString &filePath);
    void TMMChanged(int index);
    void categoryChanged(int index);
    void doNotDeleteTorrentClicked(bool checked);

    void accept() override;
    void reject() override;

private:
    explicit AddNewTorrentDialog(const BitTorrent::AddTorrentParams &inParams, QWidget *parent);
    bool loadTorrent(const QString &torrentPath);
    bool loadMagnet(const BitTorrent::MagnetUri &magnetUri);
    void populateSavePathComboBox();
    void saveSavePathHistory() const;
    int indexOfSavePath(const QString &save_path);
    void loadState();
    void saveState();
    void setMetadataProgressIndicator(bool visibleIndicator, const QString &labelText = QString());
    void setupTreeview();
    void setCommentText(const QString &str) const;
    void setSavePath(const QString &newPath);
    static CachedSettingValue<int> &savePathHistoryLengthSetting();

    void showEvent(QShowEvent *event) override;

    Ui::AddNewTorrentDialog *ui;
    TorrentContentFilterModel *m_contentModel;
    PropListDelegate *m_contentDelegate;
    bool m_hasMetadata;
    QString m_filePath;
    BitTorrent::InfoHash m_hash;
    BitTorrent::TorrentInfo m_torrentInfo;
    QByteArray m_headerState;
    int m_oldIndex;
    QScopedPointer<TorrentFileGuard> m_torrentGuard;
    BitTorrent::AddTorrentParams m_torrentParams;
};

#endif // ADDNEWTORRENTDIALOG_H
