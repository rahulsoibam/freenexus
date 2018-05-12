#ifndef PROGRAMUPDATER_H
#define PROGRAMUPDATER_H

#include <QObject>
#include <QUrl>

class ProgramUpdater: public QObject
{
    Q_OBJECT

public:
    explicit ProgramUpdater(QObject *parent = 0, bool invokedByUser = false);

    void checkForUpdates();
    void updateProgram();

signals:
    void updateCheckFinished(bool updateAvailable, QString version, bool invokedByUser);

private slots:
    void rssDownloadFinished(const QString &url, const QByteArray &data);
    void rssDownloadFailed(const QString &url, const QString &error);

private:
    bool isVersionMoreRecent(const QString &remoteVersion) const;

    QString m_updateUrl;
    bool m_invokedByUser;
};

#endif // PROGRAMUPDATER_H
