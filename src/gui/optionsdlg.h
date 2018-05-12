#ifndef OPTIONSDLG_H
#define OPTIONSDLG_H

#include <QButtonGroup>
#include <QDialog>

class QAbstractButton;
class QCloseEvent;
class QListWidgetItem;
class AdvancedSettings;

// actions on double-click on torrents
enum DoubleClickAction
{
    TOGGLE_PAUSE,
    OPEN_DEST,
    NO_ACTION
};

namespace Net
{
    enum class ProxyType;
}

namespace Ui
{
    class OptionsDialog;
}

class OptionsDialog: public QDialog
{
    Q_OBJECT
    using ThisType = OptionsDialog;

private:
    enum Tabs
    {
        TAB_UI,
        TAB_DOWNLOADS,
        TAB_CONNECTION,
        TAB_SPEED,
        TAB_BITTORRENT,
        TAB_RSS,
        TAB_WEBUI,
        TAB_ADVANCED
    };

public:
    // Constructor / Destructor
    OptionsDialog(QWidget *parent = 0);
    ~OptionsDialog();

public slots:
    void showConnectionTab();

private slots:
    void enableForceProxy(bool enable);
    void enableProxy(int index);
    void on_buttonBox_accepted();
    void closeEvent(QCloseEvent *e);
    void on_buttonBox_rejected();
    void applySettings(QAbstractButton *button);
    void enableApplyButton();
    void toggleComboRatioLimitAct();
    void changePage(QListWidgetItem *, QListWidgetItem *);
    void loadWindowState();
    void loadSplitterState();
    void saveWindowState() const;
    void handleScanFolderViewSelectionChanged();
    void on_IpFilterRefreshBtn_clicked();
    void handleIPFilterParsed(bool error, int ruleCount);
    void on_banListButton_clicked();
    void on_IPSubnetWhitelistButton_clicked();
    void on_randomButton_clicked();
    void on_addScanFolderButton_clicked();
    void on_removeScanFolderButton_clicked();
    void on_btnWebUiCrt_clicked();
    void on_btnWebUiKey_clicked();
    void on_registerDNSBtn_clicked();
    void setLocale(const QString &localeStr);

private:
    // Methods
    void saveOptions();
    void loadOptions();
    void initializeLanguageCombo();
    static QString languageToLocalizedString(const QLocale &locale);
    // General options
    QString getLocale() const;
#ifndef Q_OS_MAC
    bool systrayIntegration() const;
    bool minimizeToTray() const;
    bool closeToTray() const;
#endif
    bool startMinimized() const;
    bool isSplashScreenDisabled() const;
    bool preventFromSuspend() const;
#ifdef Q_OS_WIN
    bool WinStartup() const;
#endif
    // Downloads
    bool preAllocateAllFiles() const;
    bool useAdditionDialog() const;
    bool addTorrentsInPause() const;
    QString getTorrentExportDir() const;
    QString getFinishedTorrentExportDir() const;
    QString askForExportDir(const QString &currentExportPath);
    int getActionOnDblClOnTorrentDl() const;
    int getActionOnDblClOnTorrentFn() const;
    // Connection options
    int getPort() const;
    bool isUPnPEnabled() const;
    QPair<int, int> getGlobalBandwidthLimits() const;
    QPair<int, int> getAltGlobalBandwidthLimits() const;
    // Bittorrent options
    int getMaxConnecs() const;
    int getMaxConnecsPerTorrent() const;
    int getMaxUploads() const;
    int getMaxUploadsPerTorrent() const;
    bool isDHTEnabled() const;
    bool isLSDEnabled() const;
    int getEncryptionSetting() const;
    qreal getMaxRatio() const;
    int getMaxSeedingMinutes() const;
    // Proxy options
    bool isProxyEnabled() const;
    bool isProxyAuthEnabled() const;
    QString getProxyIp() const;
    unsigned short getProxyPort() const;
    QString getProxyUsername() const;
    QString getProxyPassword() const;
    Net::ProxyType getProxyType() const;
    // IP Filter
    bool isIPFilteringEnabled() const;
    QString getFilter() const;
    bool m_refreshingIpFilter;
    // Queueing system
    bool isQueueingSystemEnabled() const;
    int getMaxActiveDownloads() const;
    int getMaxActiveUploads() const;
    int getMaxActiveTorrents() const;
    bool isWebUiEnabled() const;
    QString webUiUsername() const;
    QString webUiPassword() const;

private:
    bool setSslKey(const QByteArray &key);
    bool setSslCertificate(const QByteArray &cert);
    bool schedTimesOk();
    bool webUIAuthenticationOk();

private:
    Ui::OptionsDialog *m_ui;
    QButtonGroup choiceLanguage;
    QAbstractButton *applyButton;
    AdvancedSettings *advancedSettings;
    QList<QString> addedScanDirs;
    QList<QString> removedScanDirs;
    // SSL Cert / key
    QByteArray m_sslCert, m_sslKey;
};

#endif
