#ifndef UTILS_GUI_H
#define UTILS_GUI_H

#include <QtGlobal>
#include <QPixmap>
#include <QSize>

class QWidget;

namespace Utils
{
    namespace Gui
    {
        void resize(QWidget *widget, const QSize &newSize = {});
        qreal screenScalingFactor(const QWidget *widget);

        template <typename T>
        T scaledSize(const QWidget *widget, const T &size)
        {
            return (size * screenScalingFactor(widget));
        }

        QPixmap scaledPixmap(const QString &path, const QWidget *widget, const int height = 0);
        QSize smallIconSize(const QWidget *widget = nullptr);
        QSize mediumIconSize(const QWidget *widget = nullptr);
        QSize largeIconSize(const QWidget *widget = nullptr);
    }
}

#endif  // UTILS_GUI_H
