#pragma once

#include <stdexcept>

#include <QDir>
#include <QFile>
#include <QObject>

class AsyncFileStorageError: public std::runtime_error
{
public:
    explicit AsyncFileStorageError(const QString &message);
    QString message() const;
};

class AsyncFileStorage: public QObject
{
    Q_OBJECT

public:
    explicit AsyncFileStorage(const QString &storageFolderPath, QObject *parent = nullptr);
    ~AsyncFileStorage() override;

    void store(const QString &fileName, const QByteArray &data);

    QDir storageDir() const;

signals:
    void failed(const QString &fileName, const QString &errorString);

private:
    Q_INVOKABLE void store_impl(const QString &fileName, const QByteArray &data);

    QDir m_storageDir;
    QFile m_lockFile;
};
