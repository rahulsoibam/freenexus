#ifndef TRACKERLIST_H
#define TRACKERLIST_H

#include <QClipboard>
#include <QList>
#include <QShortcut>
#include <QTreeWidget>

#include "propertieswidget.h"

#define NB_STICKY_ITEM 3

namespace BitTorrent
{
    class TorrentHandle;
}

class TrackerList : public QTreeWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(TrackerList)

public:
    enum TrackerListColumn
    {
        COL_TIER,
        COL_URL,
        COL_STATUS,
        COL_RECEIVED,
        COL_SEEDS,
        COL_PEERS,
        COL_DOWNLOADED,
        COL_MSG,

        COL_COUNT
    };

    TrackerList(PropertiesWidget *properties);
    ~TrackerList();

    int visibleColumnsCount() const;

public slots:
    void setRowColor(int row, QColor color);

    void moveSelectionUp();
    void moveSelectionDown();

    void clear();
    void loadStickyItems(BitTorrent::TorrentHandle *const torrent);
    void loadTrackers();
    void askForTrackers();
    void copyTrackerUrl();
    void reannounceSelected();
    void deleteSelectedTrackers();
    void editSelectedTracker();
    void showTrackerListMenu(QPoint);
    void displayToggleColumnsMenu(const QPoint &);
    void loadSettings();
    void saveSettings() const;

protected:
    QList<QTreeWidgetItem *> getSelectedTrackerItems() const;

private:
    PropertiesWidget *m_properties;
    QHash<QString, QTreeWidgetItem *> m_trackerItems;
    QTreeWidgetItem *m_DHTItem;
    QTreeWidgetItem *m_PEXItem;
    QTreeWidgetItem *m_LSDItem;
    QShortcut *m_editHotkey;
    QShortcut *m_deleteHotkey;
    QShortcut *m_copyHotkey;

    static QStringList headerLabels();
};

#endif // TRACKERLIST_H
