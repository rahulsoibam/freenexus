#include "asyncfilestorage.h"

#include <QDebug>
#include <QMetaObject>
#include <QSaveFile>

AsyncFileStorage::AsyncFileStorage(const QString &storageFolderPath, QObject *parent)
    : QObject(parent)
    , m_storageDir(storageFolderPath)
    , m_lockFile(m_storageDir.absoluteFilePath(QStringLiteral("storage.lock")))
{
    if (!m_storageDir.mkpath(m_storageDir.absolutePath()))
        throw AsyncFileStorageError(
                QString("Could not create directory '%1'.").arg(m_storageDir.absolutePath()));

    // TODO: This folder locking approach does not work for UNIX systems. Implement it.
    if (!m_lockFile.open(QFile::WriteOnly))
        throw AsyncFileStorageError(m_lockFile.errorString());
}

AsyncFileStorage::~AsyncFileStorage()
{
    m_lockFile.close();
    m_lockFile.remove();
}

void AsyncFileStorage::store(const QString &fileName, const QByteArray &data)
{
    QMetaObject::invokeMethod(this, "store_impl", Qt::QueuedConnection
                              , Q_ARG(QString, fileName), Q_ARG(QByteArray, data));
}

QDir AsyncFileStorage::storageDir() const
{
    return m_storageDir;
}

void AsyncFileStorage::store_impl(const QString &fileName, const QByteArray &data)
{
    const QString filePath = m_storageDir.absoluteFilePath(fileName);
    QSaveFile file(filePath);
    qDebug() << "AsyncFileStorage: Saving data to" << filePath;
    if (file.open(QIODevice::WriteOnly)) {
        file.write(data);
        if (!file.commit()) {
            qDebug() << "AsyncFileStorage: Failed to save data";
            emit failed(filePath, file.errorString());
        }
    }
}

AsyncFileStorageError::AsyncFileStorageError(const QString &message)
    : std::runtime_error(message.toUtf8().data())
{
}

QString AsyncFileStorageError::message() const
{
    return what();
}
