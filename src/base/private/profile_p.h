#ifndef QBT_PROFILE_P_H
#define QBT_PROFILE_P_H

#include <QDir>
#include <QStandardPaths>
#include "base/profile.h"

namespace Private
{
    class Profile
    {
    public:
        virtual QString baseDirectory() const = 0;
        virtual QString cacheLocation() const = 0;
        virtual QString configLocation() const = 0;
        virtual QString dataLocation() const = 0;
        virtual QString downloadLocation() const = 0;
        virtual SettingsPtr applicationSettings(const QString &name) const = 0;

        virtual ~Profile() = default;

        /**
         * @brief QCoreApplication::applicationName() with optional configuration name appended
         */
        QString profileName() const;

    protected:
        Profile(const QString &configurationName);

        QString configurationSuffix() const;
    private:
        QString m_configurationSuffix;
    };

    /// Default implementation. Takes paths from system
    class DefaultProfile: public Profile
    {
    public:
        DefaultProfile(const QString &configurationName);

        QString baseDirectory() const override;
        QString cacheLocation() const override;
        QString configLocation() const override;
        QString dataLocation() const override;
        QString downloadLocation() const override;
        SettingsPtr applicationSettings(const QString &name) const override;

    private:
        /**
         * @brief Standard path writable location for profile files
         *
         * @param location location kind
         * @return QStandardPaths::writableLocation(location) / configurationName()
         */
        QString locationWithConfigurationName(QStandardPaths::StandardLocation location) const;
    };

    /// Custom tree: creates directories under the specified root directory
    class CustomProfile: public Profile
    {
    public:
        CustomProfile(const QString &rootPath, const QString &configurationName);

        QString baseDirectory() const override;
        QString cacheLocation() const override;
        QString configLocation() const override;
        QString dataLocation() const override;
        QString downloadLocation() const override;
        SettingsPtr applicationSettings(const QString &name) const override;

    private:
        QDir m_rootDirectory;
        static constexpr const char *cacheDirName = "cache";
        static constexpr const char *configDirName = "config";
        static constexpr const char *dataDirName = "data";
        static constexpr const char *downloadsDirName = "downloads";
    };

    class PathConverter
    {
    public:
        virtual QString toPortablePath(const QString &path) const = 0;
        virtual QString fromPortablePath(const QString &portablePath) const = 0;
        virtual ~PathConverter() = default;
    };

    class NoConvertConverter: public PathConverter
    {
    public:
        QString toPortablePath(const QString &path) const override;
        QString fromPortablePath(const QString &portablePath) const override;
    };

    class Converter: public PathConverter
    {
    public:
        Converter(const QString &basePath);
        QString toPortablePath(const QString &path) const override;
        QString fromPortablePath(const QString &portablePath) const override;

    private:
        QDir m_baseDir;
    };
}
#endif // QBT_PROFILE_P_H
