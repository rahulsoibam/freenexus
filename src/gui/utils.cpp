#include "utils.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>
#include <QStyle>
#include <QWidget>
#include <QWindow>

void Utils::Gui::resize(QWidget *widget, const QSize &newSize)
{
    if (newSize.isValid())
        widget->resize(newSize);
    else  // depends on screen DPI
        widget->resize(widget->size() * screenScalingFactor(widget));
}

qreal Utils::Gui::screenScalingFactor(const QWidget *widget)
{
#ifdef Q_OS_WIN
    const int screen = qApp->desktop()->screenNumber(widget);
    return (QApplication::screens()[screen]->logicalDotsPerInch() / 96);
#else
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    return widget->devicePixelRatioF();
#else
    return widget->devicePixelRatio();
#endif
#endif  // Q_OS_WIN
}

QPixmap Utils::Gui::scaledPixmap(const QString &path, const QWidget *widget, const int height)
{
    const QPixmap pixmap(path);
    const int scaledHeight = ((height > 0) ? height : pixmap.height()) * Utils::Gui::screenScalingFactor(widget);
    return pixmap.scaledToHeight(scaledHeight, Qt::SmoothTransformation);
}

QSize Utils::Gui::smallIconSize(const QWidget *widget)
{
    // Get DPI scaled icon size (device-dependent), see QT source
    // under a 1080p screen is usually 16x16
    const int s = QApplication::style()->pixelMetric(QStyle::PM_SmallIconSize, nullptr, widget);
    return QSize(s, s);
}

QSize Utils::Gui::mediumIconSize(const QWidget *widget)
{
    // under a 1080p screen is usually 24x24
    return ((smallIconSize(widget) + largeIconSize(widget)) / 2);
}

QSize Utils::Gui::largeIconSize(const QWidget *widget)
{
    // Get DPI scaled icon size (device-dependent), see QT source
    // under a 1080p screen is usually 32x32
    const int s = QApplication::style()->pixelMetric(QStyle::PM_LargeIconSize, nullptr, widget);
    return QSize(s, s);
}
