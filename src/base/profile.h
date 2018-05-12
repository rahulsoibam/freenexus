#ifndef QBT_PROFILE_H
#define QBT_PROFILE_H

#include <memory>

#include <QString>
#include <QScopedPointer>
#include <QSettings>

class Application;

namespace Private
{
    class Profile;
    class PathConverter;
}

using SettingsPtr = std::unique_ptr<QSettings>;

enum class SpecialFolder
{
    Cache,
    Config,
    Data,
    Downloads
};

class Profile
{
public:
    QString location(SpecialFolder folder) const;
    SettingsPtr applicationSettings(const QString &name) const;

    /// Returns either default name for configuration file (QCoreApplication::applicationName())
    /// or the value, supplied via parameters
    QString profileName() const;

    QString toPortablePath(const QString &absolutePath) const;
    QString fromPortablePath(const QString &portablePath) const;

    static const Profile &instance();

private:
    Profile(Private::Profile *impl, Private::PathConverter *pathConverter);
    ~Profile();

    friend class ::Application;
    static void initialize(const QString &rootProfilePath, const QString &configurationName,
                                             bool convertPathsToProfileRelative);
    void ensureDirectoryExists(SpecialFolder folder);

    QScopedPointer<Private::Profile> m_profileImpl;
    QScopedPointer<Private::PathConverter> m_pathConverterImpl;
    static Profile *m_instance;
};

inline QString specialFolderLocation(SpecialFolder folder)
{
    return Profile::instance().location(folder);
}

#endif // QBT_PROFILE_H
