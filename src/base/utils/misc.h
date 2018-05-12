#ifndef UTILS_MISC_H
#define UTILS_MISC_H

#include <ctime>
#include <vector>

#include <QtGlobal>

#ifdef Q_OS_WIN
#include <memory>
#include <Windows.h>
#endif

#include <QDir>
#include <QFile>
#include <QPoint>
#include <QSize>
#include <QString>
#include <QStringList>
#include <QUrl>

#include "base/types.h"

/*  Miscellaneous functions that can be useful */

namespace Utils
{
    namespace Misc
    {
        // use binary prefix standards from IEC 60027-2
        // see http://en.wikipedia.org/wiki/Kilobyte
        enum class SizeUnit
        {
            Byte,       // 1024^0,
            KibiByte,   // 1024^1,
            MebiByte,   // 1024^2,
            GibiByte,   // 1024^3,
            TebiByte,   // 1024^4,
            PebiByte,   // 1024^5,
            ExbiByte    // 1024^6,
            // int64 is used for sizes and thus the next units can not be handled
            // ZebiByte,   // 1024^7,
            // YobiByte,   // 1024^8
        };

        QString parseHtmlLinks(const QString &raw_text);
        bool isUrl(const QString &s);

        void shutdownComputer(const ShutdownDialogAction &action);

        QString osName();
        QString boostVersionString();
        QString libtorrentVersionString();

        int pythonVersion();
        QString pythonExecutable();
        QString pythonVersionComplete();

        QString unitString(SizeUnit unit);

        // return best user friendly storage unit (B, KiB, MiB, GiB, TiB)
        // value must be given in bytes
        bool friendlyUnit(qint64 sizeInBytes, qreal& val, SizeUnit& unit);
        QString friendlyUnit(qint64 bytesValue, bool isSpeed = false);
        int friendlyUnitPrecision(SizeUnit unit);
        qint64 sizeInBytes(qreal size, SizeUnit unit);

        bool isPreviewable(const QString& extension);

        // Take a number of seconds and return an user-friendly
        // time duration like "1d 2h 10m".
        QString userFriendlyDuration(qlonglong seconds);
        QString getUserIDString();

        // Convert functions
        QStringList toStringList(const QList<bool> &l);
        QList<int> intListfromStringList(const QStringList &l);
        QList<bool> boolListfromStringList(const QStringList &l);

#ifndef DISABLE_GUI
        void openPath(const QString& absolutePath);
        void openFolderSelect(const QString& absolutePath);

        QPoint screenCenter(const QWidget *w);
#endif

#ifdef Q_OS_WIN
        QString windowsSystemPath();

        template <typename T>
        T loadWinAPI(const QString &source, const char *funcName)
        {
            QString path = windowsSystemPath();
            if (!path.endsWith('\\'))
                path += '\\';

            path += source;

            std::unique_ptr<wchar_t[]> pathWchar(new wchar_t[path.length() + 1] {});
            path.toWCharArray(pathWchar.get());

            return reinterpret_cast<T>(
                ::GetProcAddress(::LoadLibraryW(pathWchar.get()), funcName));
        }
#endif
    }
}

#endif
