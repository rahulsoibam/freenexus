#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include "filelogger.h"
#include "base/logger.h"
#include "base/utils/fs.h"

FileLogger::FileLogger(const QString &path, const bool backup, const int maxSize, const bool deleteOld, const int age, const FileLogAgeType ageType)
    : m_backup(backup)
    , m_maxSize(maxSize)
    , m_logFile(nullptr)
{
    m_flusher.setInterval(0);
    m_flusher.setSingleShot(true);
    connect(&m_flusher, SIGNAL(timeout()), SLOT(flushLog()));

    changePath(path);
    if (deleteOld)
        this->deleteOld(age, ageType);

    const Logger* const logger = Logger::instance();
    foreach (const Log::Msg& msg, logger->getMessages())
        addLogMessage(msg);

    connect(logger, SIGNAL(newLogMessage(const Log::Msg &)), SLOT(addLogMessage(const Log::Msg &)));
}

FileLogger::~FileLogger()
{
    if (!m_logFile) return;
    closeLogFile();
    delete m_logFile;
}

void FileLogger::changePath(const QString& newPath)
{
    QString tmpPath = Utils::Fs::fromNativePath(newPath);
    QDir dir(tmpPath);
    dir.mkpath(tmpPath);
    tmpPath = dir.absoluteFilePath("freenexus.log");

    if (tmpPath != m_path) {
        m_path = tmpPath;

        if (m_logFile) {
            closeLogFile();
            delete m_logFile;
        }
        m_logFile = new QFile(m_path);
        openLogFile();
    }
}

void FileLogger::deleteOld(const int age, const FileLogAgeType ageType)
{
    QDateTime date = QDateTime::currentDateTime();
    QDir dir(m_path);

    switch (ageType) {
    case DAYS:
        date = date.addDays(age);
        break;
    case MONTHS:
        date = date.addMonths(age);
        break;
    default:
        date = date.addYears(age);
    }

    foreach (const QFileInfo file, dir.entryInfoList(QStringList("freenexus.log.bak*"), QDir::Files | QDir::Writable, QDir::Time | QDir::Reversed)) {
        if (file.lastModified() < date)
            break;
        Utils::Fs::forceRemove(file.absoluteFilePath());
    }
}

void FileLogger::setBackup(bool value)
{
    m_backup = value;
}

void FileLogger::setMaxSize(int value)
{
    m_maxSize = value;
}

void FileLogger::addLogMessage(const Log::Msg &msg)
{
    if (!m_logFile) return;

    QTextStream str(m_logFile);

    switch (msg.type) {
    case Log::INFO:
        str << "(I) ";
        break;
    case Log::WARNING:
        str << "(W) ";
        break;
    case Log::CRITICAL:
        str << "(C) ";
        break;
    default:
        str << "(N) ";
    }

    str << QDateTime::fromMSecsSinceEpoch(msg.timestamp).toString(Qt::ISODate) << " - " << msg.message << endl;

    if (m_backup && (m_logFile->size() >= m_maxSize)) {
        closeLogFile();
        int counter = 0;
        QString backupLogFilename = m_path + ".bak";

        while (QFile::exists(backupLogFilename)) {
            ++counter;
            backupLogFilename = m_path + ".bak" + QString::number(counter);
        }

        QFile::rename(m_path, backupLogFilename);
        openLogFile();
    }
    else {
        m_flusher.start();
    }
}

void FileLogger::flushLog()
{
    if (m_logFile)
        m_logFile->flush();
}

void FileLogger::openLogFile()
{
    if (!m_logFile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)
        || !m_logFile->setPermissions(QFile::ReadOwner | QFile::WriteOwner)) {
        delete m_logFile;
        m_logFile = nullptr;
        Logger::instance()->addMessage(tr("An error occurred while trying to open the log file. Logging to file is disabled."), Log::CRITICAL);
    }
}

void FileLogger::closeLogFile()
{
    m_flusher.stop();
    m_logFile->close();
}
