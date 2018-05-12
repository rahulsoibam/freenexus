#ifndef RESUMEDATASAVINGMANAGER_H
#define RESUMEDATASAVINGMANAGER_H

#include <QByteArray>
#include <QDir>
#include <QObject>

class ResumeDataSavingManager : public QObject
{
    Q_OBJECT

public:
    explicit ResumeDataSavingManager(const QString &resumeFolderPath);

public slots:
    void saveResumeData(QString infoHash, QByteArray data) const;

private:
    QDir m_resumeDataDir;
};

#endif // RESUMEDATASAVINGMANAGER_H
