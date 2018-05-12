#ifndef DOWNLOADFROMURL_H
#define DOWNLOADFROMURL_H

#include <QClipboard>
#include <QDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QRegExp>
#include <QString>
#include <QStringList>

#include "ui_downloadfromurldlg.h"
#include "utils.h"

class downloadFromURL : public QDialog, private Ui::downloadFromURL
{
  Q_OBJECT

  public:
    downloadFromURL(QWidget *parent): QDialog(parent)
    {
      setupUi(this);
      setAttribute(Qt::WA_DeleteOnClose);
      setModal(true);

      buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Download"));
      connect(buttonBox, &QDialogButtonBox::accepted, this, &downloadFromURL::downloadButtonClicked);
      connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

      textUrls->setWordWrapMode(QTextOption::NoWrap);

      // Paste clipboard if there is an URL in it
      QString clip_txt = qApp->clipboard()->text();
      QStringList clip_txt_list = clip_txt.split(QLatin1Char('\n'));
      clip_txt.clear();
      QStringList clip_txt_list_cleaned;
      foreach (clip_txt, clip_txt_list) {
        clip_txt = clip_txt.trimmed();
        if (!clip_txt.isEmpty()) {
          if (clip_txt_list_cleaned.indexOf(QRegExp(clip_txt, Qt::CaseInsensitive, QRegExp::FixedString)) < 0) {
            if (clip_txt.startsWith("http://", Qt::CaseInsensitive)
                || clip_txt.startsWith("https://", Qt::CaseInsensitive)
                || clip_txt.startsWith("ftp://", Qt::CaseInsensitive)
                || clip_txt.startsWith("magnet:", Qt::CaseInsensitive)
                || clip_txt.startsWith("bc://bt/", Qt::CaseInsensitive)
                || (clip_txt.size() == 40 && !clip_txt.contains(QRegExp("[^0-9A-Fa-f]")))
                || (clip_txt.size() == 32 && !clip_txt.contains(QRegExp("[^2-7A-Za-z]")))) {
              clip_txt_list_cleaned << clip_txt;
            }
          }
        }
      }
      if (clip_txt_list_cleaned.size() > 0)
        textUrls->setText(clip_txt_list_cleaned.join("\n"));

      Utils::Gui::resize(this);
      show();
    }

    ~downloadFromURL() {}

  signals:
    void urlsReadyToBeDownloaded(const QStringList& torrent_urls);

  private slots:
    void downloadButtonClicked()
    {
      QString urls = textUrls->toPlainText();
      QStringList url_list = urls.split(QLatin1Char('\n'));
      QString url;
      QStringList url_list_cleaned;
      foreach (url, url_list) {
        url = url.trimmed();
        if (!url.isEmpty()) {
          if (url_list_cleaned.indexOf(QRegExp(url, Qt::CaseInsensitive, QRegExp::FixedString)) < 0) {
            url_list_cleaned << url;
          }
        }
      }
      if (url_list_cleaned.isEmpty()) {
        QMessageBox::warning(this, tr("No URL entered"), tr("Please type at least one URL."));
        return;
      }
      emit urlsReadyToBeDownloaded(url_list_cleaned);
      qDebug("Emitted urlsReadytobedownloaded signal");
      accept();
    }
};

#endif
