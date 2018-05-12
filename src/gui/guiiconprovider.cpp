#include "guiiconprovider.h"
#include "base/preferences.h"

#include <QIcon>
#if (defined(Q_OS_UNIX) && !defined(Q_OS_MAC))
#include <QDir>
#include <QFile>
#endif

GuiIconProvider::GuiIconProvider(QObject *parent)
    : IconProvider(parent)
{
    configure();
    connect(Preferences::instance(), SIGNAL(changed()), SLOT(configure()));
}

GuiIconProvider::~GuiIconProvider() = default;

void GuiIconProvider::initInstance()
{
    if (!m_instance)
        m_instance = new GuiIconProvider;
}

GuiIconProvider *GuiIconProvider::instance()
{
    return static_cast<GuiIconProvider *>(m_instance);
}

QIcon GuiIconProvider::getIcon(const QString &iconId)
{
    return getIcon(iconId, iconId);
}

QIcon GuiIconProvider::getIcon(const QString &iconId, const QString &fallback)
{
#if (defined(Q_OS_UNIX) && !defined(Q_OS_MAC))
    if (m_useSystemTheme) {
        QIcon icon = QIcon::fromTheme(iconId);
        if (icon.name() != iconId)
            icon = QIcon::fromTheme(fallback, QIcon(IconProvider::getIconPath(iconId)));
        icon = generateDifferentSizes(icon);
        return icon;
    }
#else
    Q_UNUSED(fallback)
#endif
    return QIcon(IconProvider::getIconPath(iconId));
}

QIcon GuiIconProvider::getFlagIcon(const QString &countryIsoCode)
{
    if (countryIsoCode.isEmpty()) return QIcon();
    return QIcon(":/icons/flags/" + countryIsoCode.toLower() + ".svg");
}

// Makes sure the icon is at least available in 16px and 24px size
// It scales the icon from the theme if necessary
// Otherwise, the UI looks broken if the icon is not available
// in the correct size.
#if (defined(Q_OS_UNIX) && !defined(Q_OS_MAC))
QIcon GuiIconProvider::generateDifferentSizes(const QIcon &icon)
{
    // if icon is loaded from SVG format, it already contains all the required sizes and we shall not resize it
    // In that case it will be available in the following sizes:
    // (QSize(16, 16), QSize(22, 22), QSize(32, 32), QSize(48, 48), QSize(64, 64), QSize(128, 128), QSize(256, 256))

    if (icon.availableSizes(QIcon::Normal, QIcon::On).size() > 6)
        return icon;

    QIcon newIcon;
    QList<QSize> requiredSizes;
    requiredSizes << QSize(16, 16) << QSize(24, 24) << QSize(32, 32);
    QList<QIcon::Mode> modes;
    modes << QIcon::Normal << QIcon::Active << QIcon::Selected << QIcon::Disabled;
    foreach (const QSize &size, requiredSizes) {
        foreach (QIcon::Mode mode, modes) {
            QPixmap pixoff = icon.pixmap(size, mode, QIcon::Off);
            if (pixoff.height() > size.height())
                pixoff = pixoff.scaled(size, Qt::KeepAspectRatio,  Qt::SmoothTransformation);
            newIcon.addPixmap(pixoff, mode, QIcon::Off);
            QPixmap pixon = icon.pixmap(size, mode, QIcon::On);
            if (pixon.height() > size.height())
                pixon = pixoff.scaled(size, Qt::KeepAspectRatio,  Qt::SmoothTransformation);
            newIcon.addPixmap(pixon, mode, QIcon::On);
        }
    }

    return newIcon;
}
#endif

QString GuiIconProvider::getIconPath(const QString &iconId)
{
#if (defined(Q_OS_UNIX) && !defined(Q_OS_MAC))
    if (m_useSystemTheme) {
        QString path = QDir::temp().absoluteFilePath(iconId + ".png");
        if (!QFile::exists(path)) {
            const QIcon icon = QIcon::fromTheme(iconId);
            if (!icon.isNull())
                icon.pixmap(32).save(path);
            else
                path = IconProvider::getIconPath(iconId);
        }

        return path;
    }
#endif
    return IconProvider::getIconPath(iconId);
}

void GuiIconProvider::configure()
{
#if (defined(Q_OS_UNIX) && !defined(Q_OS_MAC))
    m_useSystemTheme = Preferences::instance()->useSystemIconTheme();
#endif
}
