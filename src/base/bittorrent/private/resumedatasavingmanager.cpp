#include <QDebug>
#include <QSaveFile>

#include "base/logger.h"
#include "base/utils/fs.h"
#include "resumedatasavingmanager.h"

ResumeDataSavingManager::ResumeDataSavingManager(const QString &resumeFolderPath)
    : m_resumeDataDir(resumeFolderPath)
{
}

void ResumeDataSavingManager::saveResumeData(QString infoHash, QByteArray data) const
{
    QString filename = QString("%1.fastresume").arg(infoHash);
    QString filepath = m_resumeDataDir.absoluteFilePath(filename);

    qDebug() << "Saving resume data in" << filepath;
    QSaveFile resumeFile(filepath);
    if (resumeFile.open(QIODevice::WriteOnly)) {
        resumeFile.write(data);
        if (!resumeFile.commit()) {
            Logger::instance()->addMessage(QString("Couldn't save resume data in %1. Error: %2")
                                           .arg(filepath, resumeFile.errorString()), Log::WARNING);
        }
    }
}
