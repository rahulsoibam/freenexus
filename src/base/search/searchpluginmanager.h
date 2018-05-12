#pragma once

#include <QHash>
#include <QMetaType>
#include <QObject>

#include "base/utils/version.h"

using PluginVersion = Utils::Version<unsigned short, 2>;
Q_DECLARE_METATYPE(PluginVersion)

struct PluginInfo
{
    QString name;
    PluginVersion version;
    QString fullName;
    QString url;
    QStringList supportedCategories;
    QString iconPath;
    bool enabled;
};

class SearchDownloadHandler;
class SearchHandler;

class SearchPluginManager : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(SearchPluginManager)

public:
    SearchPluginManager();
    ~SearchPluginManager() override;

    static SearchPluginManager *instance();

    QStringList allPlugins() const;
    QStringList enabledPlugins() const;
    QStringList supportedCategories() const;
    QStringList getPluginCategories(const QString &pluginName) const;
    PluginInfo *pluginInfo(const QString &name) const;

    void enablePlugin(const QString &name, bool enabled = true);
    void updatePlugin(const QString &name);
    void installPlugin(const QString &source);
    bool uninstallPlugin(const QString &name);
    static void updateIconPath(PluginInfo * const plugin);
    void checkForUpdates();

    SearchHandler *startSearch(const QString &pattern, const QString &category, const QStringList &usedPlugins);
    SearchDownloadHandler *downloadTorrent(const QString &siteUrl, const QString &url);

    static PluginVersion getPluginVersion(QString filePath);
    static QString categoryFullName(const QString &categoryName);
    QString pluginFullName(const QString &pluginName);
    static QString pluginsLocation();
    static QString engineLocation();

signals:
    void pluginEnabled(const QString &name, bool enabled);
    void pluginInstalled(const QString &name);
    void pluginInstallationFailed(const QString &name, const QString &reason);
    void pluginUninstalled(const QString &name);
    void pluginUpdated(const QString &name);
    void pluginUpdateFailed(const QString &name, const QString &reason);

    void checkForUpdatesFinished(const QHash<QString, PluginVersion> &updateInfo);
    void checkForUpdatesFailed(const QString &reason);

private:
    void update();
    void updateNova();
    void parseVersionInfo(const QByteArray &info);
    void installPlugin_impl(const QString &name, const QString &path);
    bool isUpdateNeeded(QString pluginName, PluginVersion newVersion) const;

    void versionInfoDownloaded(const QString &url, const QByteArray &data);
    void versionInfoDownloadFailed(const QString &url, const QString &reason);
    void pluginDownloaded(const QString &url, QString filePath);
    void pluginDownloadFailed(const QString &url, const QString &reason);

    static QString pluginPath(const QString &name);

    static QPointer<SearchPluginManager> m_instance;
    static const QHash<QString, QString> m_categoryNames;

    const QString m_updateUrl;

    QHash<QString, PluginInfo*> m_plugins;
};
