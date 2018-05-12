#ifndef SHUTDOWNCONFIRMDLG_H
#define SHUTDOWNCONFIRMDLG_H

#include <QDialog>
#include <QTimer>

#include "base/types.h"

namespace Ui
{
    class confirmShutdownDlg;
}

class ShutdownConfirmDlg : public QDialog
{
    Q_OBJECT

public:
    ShutdownConfirmDlg(QWidget *parent, const ShutdownDialogAction &action);
    ~ShutdownConfirmDlg();

    static bool askForConfirmation(QWidget *parent, const ShutdownDialogAction &action);

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void updateSeconds();
    void accept() override;

private:
    // Methods
    void initText();
    void updateText();

    // Vars
    Ui::confirmShutdownDlg *m_ui;
    QTimer m_timer;
    int m_timeout;
    ShutdownDialogAction m_action;
    QString m_msg;
};

#endif // SHUTDOWNCONFIRM_H
