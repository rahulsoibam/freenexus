#ifndef TRANSFERLISTFILTERSWIDGET_H
#define TRANSFERLISTFILTERSWIDGET_H

#include <QListWidget>
#include <QFrame>

QT_BEGIN_NAMESPACE
class QResizeEvent;
class QCheckBox;
QT_END_NAMESPACE

class TransferListWidget;

namespace BitTorrent
{
    class TorrentHandle;
    class TrackerEntry;
}

class FiltersBase: public QListWidget
{
    Q_OBJECT

public:
    FiltersBase(QWidget *parent, TransferListWidget *transferList);

    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;

public slots:
    void toggleFilter(bool checked);

protected:
    TransferListWidget *transferList;

private slots:
    virtual void showMenu(QPoint) = 0;
    virtual void applyFilter(int row) = 0;
    virtual void handleNewTorrent(BitTorrent::TorrentHandle *const) = 0;
    virtual void torrentAboutToBeDeleted(BitTorrent::TorrentHandle *const) = 0;
};

class StatusFiltersWidget: public FiltersBase
{
    Q_OBJECT

public:
    StatusFiltersWidget(QWidget *parent, TransferListWidget *transferList);
    ~StatusFiltersWidget();

private slots:
    void updateTorrentNumbers();

private:
    // These 4 methods are virtual slots in the base class.
    // No need to redeclare them here as slots.
    virtual void showMenu(QPoint);
    virtual void applyFilter(int row);
    virtual void handleNewTorrent(BitTorrent::TorrentHandle *const);
    virtual void torrentAboutToBeDeleted(BitTorrent::TorrentHandle *const);
};

class TrackerFiltersList: public FiltersBase
{
    Q_OBJECT

public:
    TrackerFiltersList(QWidget *parent, TransferListWidget *transferList);
    ~TrackerFiltersList();

    // Redefine addItem() to make sure the list stays sorted
    void addItem(const QString &tracker, const QString &hash);
    void removeItem(const QString &tracker, const QString &hash);
    void changeTrackerless(bool trackerless, const QString &hash);
    void setDownloadTrackerFavicon(bool value);

public slots:
    void trackerSuccess(const QString &hash, const QString &tracker);
    void trackerError(const QString &hash, const QString &tracker);
    void trackerWarning(const QString &hash, const QString &tracker);

private slots:
    void handleFavicoDownload(const QString &url, const QString &filePath);
    void handleFavicoFailure(const QString &url, const QString &error);

private:
    // These 4 methods are virtual slots in the base class.
    // No need to redeclare them here as slots.
    virtual void showMenu(QPoint);
    virtual void applyFilter(int row);
    virtual void handleNewTorrent(BitTorrent::TorrentHandle *const torrent);
    virtual void torrentAboutToBeDeleted(BitTorrent::TorrentHandle *const torrent);
    QString trackerFromRow(int row) const;
    int rowFromTracker(const QString &tracker) const;
    QString getHost(const QString &tracker) const;
    QStringList getHashes(int row);
    void downloadFavicon(const QString &url);

private:
    QHash<QString, QStringList> m_trackers;
    QHash<QString, QStringList> m_errors;
    QHash<QString, QStringList> m_warnings;
    QStringList m_iconPaths;
    int m_totalTorrents;
    bool m_downloadTrackerFavicon;
};

class CategoryFilterWidget;
class TagFilterWidget;

class TransferListFiltersWidget: public QFrame
{
    Q_OBJECT

public:
    TransferListFiltersWidget(QWidget *parent, TransferListWidget *transferList);
    void setDownloadTrackerFavicon(bool value);

public slots:
    void addTrackers(BitTorrent::TorrentHandle *const torrent, const QList<BitTorrent::TrackerEntry> &trackers);
    void removeTrackers(BitTorrent::TorrentHandle *const torrent, const QList<BitTorrent::TrackerEntry> &trackers);
    void changeTrackerless(BitTorrent::TorrentHandle *const torrent, bool trackerless);
    void trackerSuccess(BitTorrent::TorrentHandle *const torrent, const QString &tracker);
    void trackerWarning(BitTorrent::TorrentHandle *const torrent, const QString &tracker);
    void trackerError(BitTorrent::TorrentHandle *const torrent, const QString &tracker);

signals:
    void trackerSuccess(const QString &hash, const QString &tracker);
    void trackerError(const QString &hash, const QString &tracker);
    void trackerWarning(const QString &hash, const QString &tracker);

private slots:
    void onCategoryFilterStateChanged(bool enabled);
    void onTagFilterStateChanged(bool enabled);

private:
    void toggleCategoryFilter(bool enabled);
    void toggleTagFilter(bool enabled);

    TransferListWidget *m_transferList;
    TrackerFiltersList *m_trackerFilters;
    CategoryFilterWidget *m_categoryFilterWidget;
    TagFilterWidget *m_tagFilterWidget;
};

#endif // TRANSFERLISTFILTERSWIDGET_H
