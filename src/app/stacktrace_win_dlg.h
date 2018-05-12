#ifndef STACKTRACE_WIN_DLG_H
#define STACKTRACE_WIN_DLG_H

#include <QString>
#include <QDialog>
#include "base/utils/misc.h"
#include "ui_stacktrace_win_dlg.h"

class StraceDlg : public QDialog, private Ui::errorDialog
{
    Q_OBJECT

public:
    StraceDlg(QWidget *parent = nullptr)
        : QDialog(parent)
    {
        setupUi(this);
    }

    void setStacktraceString(const QString &sigName, const QString &trace)
    {
        // try to call Qt function as less as possible
        QString htmlStr = QString(
            "<p align=center><b><font size=7 color=red>"
            "freeNexus has crashed"
            "</font></b></p>"
            "<font size=4><p>"
            "Please file a bug report at "
            "<a href=\"http://bugs.freenexus.org\">http://bugs.freenexus.org</a> "
            "and provide the following information:"
            "</p></font>"
            "<br/><hr><br/>"
            "<p align=center><font size=4>"
#if defined(__x86_64__) || defined(_M_X64)
            "freeNexus version: " QBT_VERSION " (64-bit)<br/>"
#else
            "freeNexus version: " QBT_VERSION " (32-bit)<br/>"
#endif
            "Libtorrent version: %1<br/>"
            "Qt version: " QT_VERSION_STR "<br/>"
            "Boost version: %2<br/>"
            "OS version: %3<br/><br/>"
            "Caught signal: %4"
            "</font></p>"
            "<pre><code>%5</code></pre>"
            "<br/><hr><br/><br/>")
                .arg(Utils::Misc::libtorrentVersionString())
                .arg(Utils::Misc::boostVersionString())
                .arg(Utils::Misc::osName())
                .arg(sigName)
                .arg(trace);

        errorText->setHtml(htmlStr);
    }
};

#endif
