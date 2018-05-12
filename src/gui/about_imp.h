#ifndef ABOUT_H
#define ABOUT_H

#include <QFile>

#include "base/utils/misc.h"
#include "base/unicodestrings.h"
#include "ui_about.h"
#include "utils.h"

class about: public QDialog, private Ui::AboutDlg
{
    Q_OBJECT

public:
    about(QWidget *parent) : QDialog(parent)
    {
        setupUi(this);
        setAttribute(Qt::WA_DeleteOnClose);

        // Title
#if defined(__x86_64__) || defined(_M_X64)
        lb_name->setText("<b><h2>freeNexus " QBT_VERSION " (64-bit)</h2></b>");
#else
        lb_name->setText("<b><h2>freeNexus " QBT_VERSION " (32-bit)</h2></b>");
#endif

        logo->setPixmap(Utils::Gui::scaledPixmap(":/icons/skin/freenexus32.png", this));

        // About
        QString aboutText = QString(
            "<p style=\"white-space: pre-wrap;\">"
            "%1\n\n"
            "%2\n\n"
            "<table>"
            "<tr><td>%3</td><td><a href=\"https:/github.com/rahulsoibam/freenexus\">https://github.com/rahulsoibam/freenexus</a></td></tr>"
            "<tr><td>%4</td><td><a href=\"http://rahulsoibam.com\">http://rahulsoibam.com</a></td></tr>"
            "<tr><td>%5</td><td><a href=\"https://github.com/rahulsoibam/freenexus/issues\">https://github.com/rahulsoibam/freenexus/issues</a></td></tr>"
            "</table>"
            "</p>")
            .arg(tr("An advanced BitTorrent client with inbuilt decentralized search functionality programmed in C++, based on Qt toolkit and libtorrent-rasterbar.")
                , tr("Copyright %1 2017-2018 The freeNexus project").arg(QString::fromUtf8(C_COPYRIGHT))
                , tr("Home Page:")
                , tr("Author\'s website:")
                , tr("Bug Tracker:"));
        lb_about->setText(aboutText);

        labelMascot->setPixmap(Utils::Gui::scaledPixmap(":/icons/skin/mascot.png", this));

        // Thanks
        QFile thanksfile(":/thanks.html");
        if (thanksfile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            te_thanks->setHtml(QString::fromUtf8(thanksfile.readAll().constData()));
            thanksfile.close();
        }

        // License
        QFile licensefile(":/gpl.html");
        if (licensefile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            te_license->setHtml(QString::fromUtf8(licensefile.readAll().constData()));
            licensefile.close();
        }

        // Libraries
        label_11->setText(QT_VERSION_STR);
        label_12->setText(Utils::Misc::libtorrentVersionString());
        label_13->setText(Utils::Misc::boostVersionString());

        Utils::Gui::resize(this);
        show();
    }
};

#endif
