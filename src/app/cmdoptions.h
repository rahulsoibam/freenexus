#ifndef APP_OPTIONS_H
#define APP_OPTIONS_H

#include <stdexcept>

#include <QString>
#include <QStringList>

#include "base/tristatebool.h"

class QProcessEnvironment;

struct QBtCommandLineParameters
{
    bool showHelp, relativeFastresumePaths, portableMode, skipChecking, sequential, firstLastPiecePriority;
#ifndef Q_OS_WIN
    bool showVersion;
#endif
#ifndef DISABLE_GUI
    bool noSplash;
#else
    bool shouldDaemonize;
#endif
    int webUiPort;
    TriStateBool addPaused, skipDialog;
    QStringList torrents;
    QString profileDir, configurationName, savePath, category, unknownParameter;

    QBtCommandLineParameters(const QProcessEnvironment&);
    QStringList paramList() const;
};

class CommandLineParameterError: public std::runtime_error
{
public:
    CommandLineParameterError(const QString &messageForUser);
    const QString& messageForUser() const;

private:
    const QString m_messageForUser;
};

QBtCommandLineParameters parseCommandLine(const QStringList &args);
void displayUsage(const QString &prgName);

#endif // APP_OPTIONS_H
