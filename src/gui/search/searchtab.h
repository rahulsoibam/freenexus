#pragma once

#include <QWidget>

#define ENGINE_URL_COLUMN 4
#define URL_COLUMN 5

class QLabel;
class QModelIndex;
class QHeaderView;
class QStandardItem;
class QStandardItemModel;
class QVBoxLayout;

template <typename T> class CachedSettingValue;

class SearchHandler;
class SearchSortModel;
class SearchListDelegate;
struct SearchResult;

namespace Ui
{
    class SearchTab;
}

class SearchTab : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SearchTab)

public:
    enum class NameFilteringMode
    {
        Everywhere,
        OnlyNames
    };
    Q_ENUM(NameFilteringMode)

    enum class Status
    {
        Ongoing,
        Finished,
        Error,
        Aborted,
        NoResults
    };

    explicit SearchTab(SearchHandler *searchHandler, QWidget *parent = nullptr);
    ~SearchTab() override;

    Status status() const;
    int visibleResultsCount() const;

    void cancelSearch();

    void downloadTorrents();
    void openTorrentPages();
    void copyTorrentURLs();

signals:
    void resultsCountUpdated();
    void statusChanged();

private:
    void loadSettings();
    void saveSettings() const;
    void updateFilter();
    void displayToggleColumnsMenu(const QPoint&);
    void onItemDoubleClicked(const QModelIndex &index);
    void searchFinished(bool cancelled);
    void searchFailed();
    void appendSearchResults(const QList<SearchResult> &results);
    void updateResultsCount();
    void setStatus(Status value);
    void downloadTorrent(const QModelIndex &rowIndex);
    void addTorrentToSession(const QString &source);
    void fillFilterComboBoxes();
    NameFilteringMode filteringMode() const;
    QHeaderView *header() const;
    void setRowColor(int row, const QColor &color);

    static QString statusText(Status st);
    static CachedSettingValue<NameFilteringMode>& nameFilteringModeSetting();

    Ui::SearchTab *m_ui;
    SearchHandler *m_searchHandler;
    QStandardItemModel *m_searchListModel;
    SearchSortModel *m_proxyModel;
    SearchListDelegate *m_searchDelegate;
    Status m_status = Status::Ongoing;
    bool m_noSearchResults = true;
};

Q_DECLARE_METATYPE(SearchTab::NameFilteringMode)
