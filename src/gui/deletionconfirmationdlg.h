#ifndef DELETIONCONFIRMATIONDLG_H
#define DELETIONCONFIRMATIONDLG_H

#include <QDialog>
#include <QPushButton>

#include "base/preferences.h"
#include "base/utils/misc.h"
#include "base/utils/string.h"
#include "guiiconprovider.h"
#include "ui_confirmdeletiondlg.h"
#include "utils.h"

class DeletionConfirmationDlg : public QDialog, private Ui::confirmDeletionDlg {
  Q_OBJECT

  public:
  DeletionConfirmationDlg(QWidget *parent, const int &size, const QString &name, bool defaultDeleteFiles): QDialog(parent) {
    setupUi(this);
    if (size == 1)
      label->setText(tr("Are you sure you want to delete '%1' from the transfer list?", "Are you sure you want to delete 'ubuntu-linux-iso' from the transfer list?").arg(name.toHtmlEscaped()));
    else
      label->setText(tr("Are you sure you want to delete these %1 torrents from the transfer list?", "Are you sure you want to delete these 5 torrents from the transfer list?").arg(QString::number(size)));
    // Icons
    const QSize iconSize = Utils::Gui::largeIconSize();
    lbl_warn->setPixmap(GuiIconProvider::instance()->getIcon("dialog-warning").pixmap(iconSize));
    lbl_warn->setFixedWidth(iconSize.width());
    rememberBtn->setIcon(GuiIconProvider::instance()->getIcon("object-locked"));
    rememberBtn->setIconSize(Utils::Gui::mediumIconSize());

    checkPermDelete->setChecked(defaultDeleteFiles || Preferences::instance()->deleteTorrentFilesAsDefault());
    connect(checkPermDelete, SIGNAL(clicked()), this, SLOT(updateRememberButtonState()));
    buttonBox->button(QDialogButtonBox::Cancel)->setFocus();

    Utils::Gui::resize(this);
  }

  bool shouldDeleteLocalFiles() const {
    return checkPermDelete->isChecked();
  }

  static bool askForDeletionConfirmation(QWidget *parent, bool& deleteLocalFiles, const int& size, const QString& name) {
    DeletionConfirmationDlg dlg(parent, size, name, deleteLocalFiles);
    if (dlg.exec() == QDialog::Accepted) {
      deleteLocalFiles = dlg.shouldDeleteLocalFiles();
      return true;
    }
    return false;
  }

private slots:
  void updateRememberButtonState() {
    rememberBtn->setEnabled(checkPermDelete->isChecked() != Preferences::instance()->deleteTorrentFilesAsDefault());
  }

  void on_rememberBtn_clicked() {
    Preferences::instance()->setDeleteTorrentFilesAsDefault(checkPermDelete->isChecked());
    rememberBtn->setEnabled(false);
  }
};

#endif // DELETIONCONFIRMATIONDLG_H
