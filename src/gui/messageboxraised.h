#ifndef MESSAGEBOXRAISED_H
#define MESSAGEBOXRAISED_H

#include <QMessageBox>

class MessageBoxRaised : public QMessageBox
{
  Q_OBJECT

private:
  MessageBoxRaised(QMessageBox::Icon icon, const QString &title, const QString &text, QMessageBox::StandardButtons buttons = NoButton, QWidget *parent = 0, Qt::WindowFlags f = Qt::Dialog|Qt::MSWindowsFixedSizeDialogHint);
  MessageBoxRaised();
  MessageBoxRaised(MessageBoxRaised const&);
  void operator=(MessageBoxRaised const&);

public:
  static QMessageBox::StandardButton critical(QWidget *parent, const QString &title, const QString &text, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);
  static QMessageBox::StandardButton information(QWidget *parent, const QString &title, const QString &text, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);
  static QMessageBox::StandardButton question(QWidget *parent, const QString &title, const QString &text, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);
  static QMessageBox::StandardButton warning(QWidget *parent, const QString &title, const QString &text, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);

protected:
  void showEvent(QShowEvent *event);

private:
  static QMessageBox::StandardButton impl(const QMessageBox::Icon &icon, QWidget *parent, const QString &title, const QString &text, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);
};

#endif // MESSAGEBOXRAISED_H
