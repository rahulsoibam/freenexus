#include "messageboxraised.h"

MessageBoxRaised::MessageBoxRaised(QMessageBox::Icon icon, const QString &title, const QString &text,
                                   QMessageBox::StandardButtons buttons, QWidget *parent, Qt::WindowFlags f)
  : QMessageBox(icon, title, text, buttons, parent, f) {}

QMessageBox::StandardButton MessageBoxRaised::impl(const QMessageBox::Icon &icon, QWidget *parent, const QString &title, const QString &text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton) {
  MessageBoxRaised dlg(icon, title, text, buttons, parent);
  dlg.setDefaultButton(defaultButton);
  return static_cast<QMessageBox::StandardButton>(dlg.exec());
}

QMessageBox::StandardButton MessageBoxRaised::critical(QWidget *parent, const QString &title, const QString &text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton) {
  return impl(Critical, parent, title, text, buttons, defaultButton);
}

QMessageBox::StandardButton MessageBoxRaised::information(QWidget *parent, const QString &title, const QString &text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton) {
  return impl(Information, parent, title, text, buttons, defaultButton);
}

QMessageBox::StandardButton MessageBoxRaised::question(QWidget *parent, const QString &title, const QString &text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton) {
  return impl(Question, parent, title, text, buttons, defaultButton);
}

QMessageBox::StandardButton MessageBoxRaised::warning(QWidget *parent, const QString &title, const QString &text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton) {
  return impl(Warning, parent, title, text, buttons, defaultButton);
}

void MessageBoxRaised::showEvent(QShowEvent *event) {
  QMessageBox::showEvent(event);
  activateWindow();
  raise();
}
