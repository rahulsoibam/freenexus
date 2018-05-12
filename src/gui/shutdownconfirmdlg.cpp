#include "shutdownconfirmdlg.h"

#include <QDialogButtonBox>
#include <QIcon>
#include <QPushButton>
#include <QStyle>

#include "base/preferences.h"
#include "base/utils/misc.h"
#include "ui_shutdownconfirmdlg.h"
#include "utils.h"

ShutdownConfirmDlg::ShutdownConfirmDlg(QWidget *parent, const ShutdownDialogAction &action)
    : QDialog(parent)
    , m_ui(new Ui::confirmShutdownDlg)
    , m_timeout(15)
    , m_action(action)
{
    m_ui->setupUi(this);

    initText();
    QIcon warningIcon(style()->standardIcon(QStyle::SP_MessageBoxWarning));
    m_ui->warningLabel->setPixmap(warningIcon.pixmap(32));

    if (m_action == ShutdownDialogAction::Exit)
        m_ui->neverShowAgainCheckbox->setVisible(true);
    else
        m_ui->neverShowAgainCheckbox->setVisible(false);

    // Cancel Button
    QPushButton *cancelButton = m_ui->buttonBox->button(QDialogButtonBox::Cancel);
    cancelButton->setFocus();
    cancelButton->setDefault(true);

    // Always on top
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    move(Utils::Misc::screenCenter(this));

    m_timer.setInterval(1000); // 1sec
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(updateSeconds()));

    Utils::Gui::resize(this);
}

ShutdownConfirmDlg::~ShutdownConfirmDlg()
{
    delete m_ui;
}

void ShutdownConfirmDlg::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    m_timer.start();
}

bool ShutdownConfirmDlg::askForConfirmation(QWidget *parent, const ShutdownDialogAction &action)
{
    ShutdownConfirmDlg dlg(parent, action);
    return (dlg.exec() == QDialog::Accepted);
}

void ShutdownConfirmDlg::updateSeconds()
{
    --m_timeout;
    updateText();
    if (m_timeout == 0) {
        m_timer.stop();
        accept();
    }
}

void ShutdownConfirmDlg::accept()
{
    Preferences::instance()->setDontConfirmAutoExit(m_ui->neverShowAgainCheckbox->isChecked());
    QDialog::accept();
}

void ShutdownConfirmDlg::initText()
{
    QPushButton *okButton = m_ui->buttonBox->button(QDialogButtonBox::Ok);

    switch (m_action) {
    case ShutdownDialogAction::Exit:
        m_msg = tr("freeNexus will now exit.");
        okButton->setText(tr("E&xit Now"));
        setWindowTitle(tr("Exit confirmation"));
        break;
    case ShutdownDialogAction::Shutdown:
        m_msg = tr("The computer is going to shutdown.");
        okButton->setText(tr("&Shutdown Now"));
        setWindowTitle(tr("Shutdown confirmation"));
        break;
    case ShutdownDialogAction::Suspend:
        m_msg = tr("The computer is going to enter suspend mode.");
        okButton->setText(tr("&Suspend Now"));
        setWindowTitle(tr("Suspend confirmation"));
        break;
    case ShutdownDialogAction::Hibernate:
        m_msg = tr("The computer is going to enter hibernation mode.");
        okButton->setText(tr("&Hibernate Now"));
        setWindowTitle(tr("Hibernate confirmation"));
        break;
    }

    m_msg += "\n";
    updateText();
}

void ShutdownConfirmDlg::updateText()
{
    QString t = tr("You can cancel the action within %1 seconds.").arg(QString::number(m_timeout)) + "\n";
    m_ui->shutdownText->setText(m_msg + t);
}
