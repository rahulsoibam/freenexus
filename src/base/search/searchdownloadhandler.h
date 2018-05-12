#pragma once

#include <QObject>

class QProcess;
class SearchPluginManager;

class SearchDownloadHandler : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(SearchDownloadHandler)

    friend class SearchPluginManager;

    SearchDownloadHandler(const QString &siteUrl, const QString &url, SearchPluginManager *manager);

signals:
    void downloadFinished(const QString &path);

private:
    void downloadProcessFinished(int exitcode);

    SearchPluginManager *m_manager;
    QProcess *m_downloadProcess;
};
