#ifndef FILELOGGER_H
#define FILELOGGER_H

#include <QObject>
#include <QTimer>

class QFile;

namespace Log
{
    struct Msg;
}

class FileLogger : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(FileLogger)

public:
    enum FileLogAgeType
    {
        DAYS,
        MONTHS,
        YEARS
    };

    FileLogger(const QString &path, const bool backup, const int maxSize, const bool deleteOld, const int age, const FileLogAgeType ageType);
    ~FileLogger();

    void changePath(const QString &newPath);
    void deleteOld(const int age, const FileLogAgeType ageType);
    void setBackup(bool value);
    void setMaxSize(int value);

private slots:
    void addLogMessage(const Log::Msg &msg);
    void flushLog();

private:
    void openLogFile();
    void closeLogFile();

    QString m_path;
    bool m_backup;
    int m_maxSize;
    QFile *m_logFile;
    QTimer m_flusher;
};

#endif // FILELOGGER_H

