#ifndef AUTOEXPANDABLEDIALOG_H
#define AUTOEXPANDABLEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QString>

namespace Ui
{
    class AutoExpandableDialog;
}

class AutoExpandableDialog: public QDialog
{
    Q_OBJECT

public:
    explicit AutoExpandableDialog(QWidget *parent);
    ~AutoExpandableDialog();

    static QString getText(QWidget *parent, const QString &title, const QString &label,
                            QLineEdit::EchoMode mode = QLineEdit::Normal, const QString &text = QString(),
                            bool *ok = 0, Qt::InputMethodHints inputMethodHints = Qt::ImhNone);

protected:
    void showEvent(QShowEvent *e);

private:
    Ui::AutoExpandableDialog *m_ui;
};

#endif // AUTOEXPANDABLEDIALOG_H
