#ifndef ADVANCEDSETTINGS_H
#define ADVANCEDSETTINGS_H

#include <QCheckBox>
#include <QComboBox>
#include <QEvent>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QTableWidget>

class WheelEventEater: public QObject
{
    Q_OBJECT

private:
    bool eventFilter(QObject *obj, QEvent *event)
    {
        switch (event->type()) {
        case QEvent::Wheel:
            return true;
        default:
            return QObject::eventFilter(obj, event);
        }
    }
};

class AdvancedSettings: public QTableWidget
{
    Q_OBJECT

public:
    AdvancedSettings(QWidget *parent);

public slots:
    void saveAdvancedSettings();

signals:
    void settingsChanged();

private slots:
    void updateCacheSpinSuffix(int value);
    void updateInterfaceAddressCombo();

private:
    void loadAdvancedSettings();
    template <typename T> void addRow(int row, const QString &rowText, T* widget);

    QLabel labelQbtLink, labelLibtorrentLink;
    QSpinBox spin_cache, spin_save_resume_data_interval, outgoing_ports_min, outgoing_ports_max, spin_list_refresh, spin_maxhalfopen, spin_tracker_port, spin_cache_ttl,
             spinSendBufferWatermark, spinSendBufferLowWatermark, spinSendBufferWatermarkFactor, spinSavePathHistoryLength;
    QCheckBox cb_os_cache, cb_recheck_completed, cb_resolve_countries, cb_resolve_hosts, cb_super_seeding,
              cb_program_notifications, cb_torrent_added_notifications, cb_tracker_favicon, cb_tracker_status,
              cb_confirm_torrent_recheck, cb_confirm_remove_all_tags, cb_listen_ipv6, cb_announce_all_trackers, cb_announce_all_tiers,
              cbGuidedReadCache, cbMultiConnectionsPerIp, cbSuggestMode, cbCoalesceRW;
    QComboBox combo_iface, combo_iface_address, comboUtpMixedMode, comboChokingAlgorithm, comboSeedChokingAlgorithm;
    QLineEdit txtAnnounceIP;

    // OS dependent settings
#if defined(Q_OS_WIN) || defined(Q_OS_MAC)
    QCheckBox cb_update_check;
#endif

#if (defined(Q_OS_UNIX) && !defined(Q_OS_MAC))
    QCheckBox cb_use_icon_theme;
#endif
};

#endif // ADVANCEDSETTINGS_H
