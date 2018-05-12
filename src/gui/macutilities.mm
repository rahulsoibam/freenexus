#include "macutilities.h"

#include <QSet>
#include <QtMac>
#include <objc/message.h>
#import <Cocoa/Cocoa.h>

namespace MacUtils
{
    QPixmap pixmapForExtension(const QString &ext, const QSize &size)
    {
        @autoreleasepool {
            NSImage *image = [[NSWorkspace sharedWorkspace] iconForFileType:ext.toNSString()];
            if (image) {
                NSRect rect = NSMakeRect(0, 0, size.width(), size.height());
                CGImageRef cgImage = [image CGImageForProposedRect:&rect context:nil hints:nil];
                return QtMac::fromCGImageRef(cgImage);
            }

            return QPixmap();
        }
    }

    void overrideDockClickHandler(bool (*dockClickHandler)(id, SEL, ...))
    {
        NSApplication *appInst = [NSApplication sharedApplication];

        if (!appInst)
            return;

        Class delClass = [[appInst delegate] class];
        SEL shouldHandle = sel_registerName("applicationShouldHandleReopen:hasVisibleWindows:");

        if (class_getInstanceMethod(delClass, shouldHandle)) {
            if (class_replaceMethod(delClass, shouldHandle, (IMP)dockClickHandler, "B@:"))
                qDebug("Registered dock click handler (replaced original method)");
            else
                qWarning("Failed to replace method for dock click handler");
        }
        else {
            if (class_addMethod(delClass, shouldHandle, (IMP)dockClickHandler, "B@:"))
                qDebug("Registered dock click handler");
            else
                qWarning("Failed to register dock click handler");
        }
    }

    void displayNotification(const QString &title, const QString &message)
    {
        @autoreleasepool {
            NSUserNotification *notification = [[NSUserNotification alloc] init];
            notification.title = title.toNSString();
            notification.informativeText = message.toNSString();
            notification.soundName = NSUserNotificationDefaultSoundName;

            [[NSUserNotificationCenter defaultUserNotificationCenter] deliverNotification:notification];
        }
    }

    void openFiles(const QSet<QString> &pathsList)
    {
        @autoreleasepool {
            NSMutableArray *pathURLs = [NSMutableArray arrayWithCapacity:pathsList.size()];

            for (const auto &path : pathsList)
                [pathURLs addObject:[NSURL fileURLWithPath:path.toNSString()]];

            [[NSWorkspace sharedWorkspace] activateFileViewerSelectingURLs:pathURLs];
        }
    }
}
