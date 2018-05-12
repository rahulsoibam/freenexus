#include "profile.h"

#include <QCoreApplication>

#include "private/profile_p.h"

Profile *Profile::m_instance = nullptr;

Profile::Profile(Private::Profile *impl, Private::PathConverter *pathConverter)
    : m_profileImpl(impl)
    , m_pathConverterImpl(pathConverter)
{
    ensureDirectoryExists(SpecialFolder::Cache);
    ensureDirectoryExists(SpecialFolder::Config);
    ensureDirectoryExists(SpecialFolder::Data);
}

// to generate correct call to ProfilePrivate::~ProfileImpl()
Profile::~Profile() = default;

void Profile::initialize(const QString &rootProfilePath, const QString &configurationName,
                         bool convertPathsToProfileRelative)
{
    QScopedPointer<Private::Profile> profile(rootProfilePath.isEmpty()
                                             ? static_cast<Private::Profile *>(new Private::DefaultProfile(configurationName))
                                             : static_cast<Private::Profile *>(new Private::CustomProfile(rootProfilePath, configurationName)));

    QScopedPointer<Private::PathConverter> converter(convertPathsToProfileRelative
                                                     ? static_cast<Private::PathConverter *>(new Private::Converter(profile->baseDirectory()))
                                                     : static_cast<Private::PathConverter *>(new Private::NoConvertConverter()));
    m_instance = new Profile(profile.take(), converter.take());
}

const Profile &Profile::instance()
{
    return *m_instance;
}

QString Profile::location(SpecialFolder folder) const
{
    QString result;
    switch (folder) {
    case SpecialFolder::Cache:
        result = m_profileImpl->cacheLocation();
        break;
    case SpecialFolder::Config:
        result = m_profileImpl->configLocation();
        break;
    case SpecialFolder::Data:
        result = m_profileImpl->dataLocation();
        break;
    case SpecialFolder::Downloads:
        result = m_profileImpl->downloadLocation();
        break;
    }

    if (!result.endsWith(QLatin1Char('/')))
        result += QLatin1Char('/');
    return result;
}

QString Profile::profileName() const
{
    return m_profileImpl->profileName();
}

SettingsPtr Profile::applicationSettings(const QString &name) const
{
    return m_profileImpl->applicationSettings(name);
}

void Profile::ensureDirectoryExists(SpecialFolder folder)
{
    QString locationPath = location(folder);
    if (!locationPath.isEmpty() && !QDir().mkpath(locationPath))
        qFatal("Could not create required directory '%s'", qUtf8Printable(locationPath));
}

QString Profile::toPortablePath(const QString &absolutePath) const
{
    return m_pathConverterImpl->toPortablePath(absolutePath);
}

QString Profile::fromPortablePath(const QString &portablePath) const
{
    return m_pathConverterImpl->fromPortablePath(portablePath);
}
