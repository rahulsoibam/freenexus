#pragma once

#include <QByteArray>
#include <QList>
#include <QObject>

class QProcess;
class QTimer;

struct SearchResult
{
    QString fileName;
    QString fileUrl;
    qlonglong fileSize;
    qlonglong nbSeeders;
    qlonglong nbLeechers;
    QString siteUrl;
    QString descrLink;
};

class SearchPluginManager;

class SearchHandler : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(SearchHandler)

    friend class SearchPluginManager;

    SearchHandler(const QString &pattern, const QString &category
                  , const QStringList &usedPlugins, SearchPluginManager *manager);

public:
    bool isActive() const;
    QString pattern() const;
    SearchPluginManager *manager() const;
    QList<SearchResult> results() const;

    void cancelSearch();

signals:
    void searchFinished(bool cancelled = false);
    void searchFailed();
    void newSearchResults(const QList<SearchResult> &results);

private:
    void readSearchOutput();
    void processFailed();
    void processFinished(int exitcode);
    bool parseSearchResult(const QString &line, SearchResult &searchResult);

    const QString m_pattern;
    const QString m_category;
    const QStringList m_usedPlugins;
    SearchPluginManager *m_manager;
    QProcess *m_searchProcess;
    QTimer *m_searchTimeout;
    QByteArray m_searchResultLineTruncated;
    bool m_searchCancelled = false;
    QList<SearchResult> m_results;
};
