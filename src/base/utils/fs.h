#ifndef UTILS_FS_H
#define UTILS_FS_H

/**
 * Utility functions related to file system.
 */

#include <QString>

namespace Utils
{
    namespace Fs
    {
        QString toNativePath(const QString &path);
        QString fromNativePath(const QString &path);
        QString fileExtension(const QString &filename);
        QString fileName(const QString &filePath);
        QString folderName(const QString &filePath);
        qint64 computePathSize(const QString &path);
        bool sameFiles(const QString &path1, const QString &path2);
        QString toValidFileSystemName(const QString &name, bool allowSeparators = false
                , const QString &pad = QLatin1String(" "));
        bool isValidFileSystemName(const QString &name, bool allowSeparators = false);
        qint64 freeDiskSpaceOnPath(const QString &path);
        QString branchPath(const QString &filePath, QString *removed = 0);
        bool sameFileNames(const QString &first, const QString &second);
        QString expandPath(const QString &path);
        QString expandPathAbs(const QString &path);
        bool isRegularFile(const QString &path);

        bool smartRemoveEmptyFolderTree(const QString &path);
        bool forceRemove(const QString &filePath);
        void removeDirRecursive(const QString &path);

        QString tempPath();

#if !defined Q_OS_WIN && !defined Q_OS_HAIKU
        bool isNetworkFileSystem(const QString &path);
#endif
    }
}

#endif // UTILS_FS_H
