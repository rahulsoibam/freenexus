#include "updownratiodlg.h"

#include <QMessageBox>

#include "base/bittorrent/session.h"
#include "ui_updownratiodlg.h"
#include "utils.h"

UpDownRatioDlg::UpDownRatioDlg(bool useDefault, qreal initialRatioValue,
                               qreal maxRatioValue, int initialTimeValue,
                               int maxTimeValue, QWidget *parent)
    : QDialog(parent)
    , m_ui(new Ui::UpDownRatioDlg)
{
    m_ui->setupUi(this);

    if (useDefault) {
        m_ui->useDefaultButton->setChecked(true);
    }
    else if ((initialRatioValue == -1.) && (initialTimeValue == -1)) {
        m_ui->noLimitButton->setChecked(true);
        initialRatioValue = BitTorrent::Session::instance()->globalMaxRatio();
        initialTimeValue = BitTorrent::Session::instance()->globalMaxSeedingMinutes();
    }
    else {
        m_ui->torrentLimitButton->setChecked(true);

        if (initialRatioValue >= 0)
            m_ui->checkMaxRatio->setChecked(true);

        if (initialTimeValue >= 0)
            m_ui->checkMaxTime->setChecked(true);
    }

    m_ui->ratioSpinBox->setMinimum(0);
    m_ui->ratioSpinBox->setMaximum(maxRatioValue);
    m_ui->ratioSpinBox->setValue(initialRatioValue);

    m_ui->timeSpinBox->setMinimum(0);
    m_ui->timeSpinBox->setMaximum(maxTimeValue);
    m_ui->timeSpinBox->setValue(initialTimeValue);

    connect(m_ui->buttonGroup, SIGNAL(buttonClicked(int)), SLOT(handleRatioTypeChanged()));
    connect(m_ui->checkMaxRatio, SIGNAL(toggled(bool)), this, SLOT(enableRatioSpin()));
    connect(m_ui->checkMaxTime, SIGNAL(toggled(bool)), this, SLOT(enableTimeSpin()));

    handleRatioTypeChanged();

    Utils::Gui::resize(this);
}

void UpDownRatioDlg::accept()
{
    if (m_ui->torrentLimitButton->isChecked() && !m_ui->checkMaxRatio->isChecked() && !m_ui->checkMaxTime->isChecked())
        QMessageBox::critical(this, tr("No share limit method selected"),
                              tr("Please select a limit method first"));
    else
        QDialog::accept();
}

bool UpDownRatioDlg::useDefault() const
{
    return m_ui->useDefaultButton->isChecked();
}

qreal UpDownRatioDlg::ratio() const
{
    return (m_ui->noLimitButton->isChecked() || !m_ui->checkMaxRatio->isChecked()) ? -1. : m_ui->ratioSpinBox->value();
}

int UpDownRatioDlg::seedingTime() const
{
    return (m_ui->noLimitButton->isChecked() || !m_ui->checkMaxTime->isChecked()) ? -1 : m_ui->timeSpinBox->value();
}

void UpDownRatioDlg::handleRatioTypeChanged()
{
    // ui->ratioSpinBox->setEnabled(ui->torrentLimitButton->isChecked());
    m_ui->checkMaxRatio->setEnabled(m_ui->torrentLimitButton->isChecked());
    m_ui->checkMaxTime->setEnabled(m_ui->torrentLimitButton->isChecked());

    m_ui->ratioSpinBox->setEnabled(m_ui->torrentLimitButton->isChecked() && m_ui->checkMaxRatio->isChecked());
    m_ui->timeSpinBox->setEnabled(m_ui->torrentLimitButton->isChecked() && m_ui->checkMaxTime->isChecked());
}

void UpDownRatioDlg::enableRatioSpin()
{
    m_ui->ratioSpinBox->setEnabled(m_ui->checkMaxRatio->isChecked());
}

void UpDownRatioDlg::enableTimeSpin()
{
    m_ui->timeSpinBox->setEnabled(m_ui->checkMaxTime->isChecked());
}

UpDownRatioDlg::~UpDownRatioDlg()
{
    delete m_ui;
}
