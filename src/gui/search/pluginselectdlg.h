#ifndef PLUGINSELECTDLG_H
#define PLUGINSELECTDLG_H

#include <QDialog>

#include "base/search/searchpluginmanager.h"

class QDropEvent;
class QStringList;
class QTreeWidgetItem;

namespace Ui
{
    class PluginSelectDlg;
}

class PluginSelectDlg: public QDialog
{
    Q_OBJECT

public:
    explicit PluginSelectDlg(SearchPluginManager *pluginManager, QWidget *parent = 0);
    ~PluginSelectDlg();

    QList<QTreeWidgetItem*> findItemsWithUrl(QString url);
    QTreeWidgetItem* findItemWithID(QString id);

protected:
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);

private slots:
    void on_actionUninstall_triggered();
    void on_updateButton_clicked();
    void on_installButton_clicked();
    void on_closeButton_clicked();
    void togglePluginState(QTreeWidgetItem*, int);
    void setRowColor(int row, QString color);
    void displayContextMenu(const QPoint& pos);
    void enableSelection(bool enable);
    void askForLocalPlugin();
    void askForPluginUrl();
    void iconDownloaded(const QString &url, QString filePath);
    void iconDownloadFailed(const QString &url, const QString &reason);

    void checkForUpdatesFinished(const QHash<QString, PluginVersion> &updateInfo);
    void checkForUpdatesFailed(const QString &reason);
    void pluginInstalled(const QString &name);
    void pluginInstallationFailed(const QString &name, const QString &reason);
    void pluginUpdated(const QString &name);
    void pluginUpdateFailed(const QString &name, const QString &reason);

private:
    void loadSupportedSearchPlugins();
    void addNewPlugin(QString pluginName);
    void startAsyncOp();
    void finishAsyncOp();
    void finishPluginUpdate();

    Ui::PluginSelectDlg *m_ui;
    SearchPluginManager *m_pluginManager;
    QStringList m_updatedPlugins;
    int m_asyncOps;
    int m_pendingUpdates;
};

#endif // PLUGINSELECTDLG_H
