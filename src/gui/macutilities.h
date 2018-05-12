#ifndef MACUTILITIES_H
#define MACUTILITIES_H

#include <QPixmap>
#include <QSize>
#include <objc/objc.h>

namespace MacUtils
{
    QPixmap pixmapForExtension(const QString &ext, const QSize &size);
    void overrideDockClickHandler(bool (*dockClickHandler)(id, SEL, ...));
    void displayNotification(const QString &title, const QString &message);
    void openFiles(const QSet<QString> &pathsList);
}

#endif // MACUTILITIES_H
