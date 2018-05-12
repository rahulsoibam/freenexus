#include "speedlimitdlg.h"

#include "base/unicodestrings.h"
#include "ui_bandwidth_limit.h"
#include "utils.h"

SpeedLimitDialog::SpeedLimitDialog(QWidget *parent)
    : QDialog(parent)
    , m_ui(new Ui::bandwidth_dlg())
{
    m_ui->setupUi(this);
    qDebug("Bandwidth allocation dialog creation");

    // Connect to slots
    connect(m_ui->bandwidthSlider, SIGNAL(valueChanged(int)), this, SLOT(updateSpinValue(int)));
    connect(m_ui->spinBandwidth, SIGNAL(valueChanged(int)), this, SLOT(updateSliderValue(int)));

    Utils::Gui::resize(this);
}

SpeedLimitDialog::~SpeedLimitDialog()
{
    qDebug("Deleting bandwidth allocation dialog");
    delete m_ui;
}

// -2: if cancel
long SpeedLimitDialog::askSpeedLimit(QWidget *parent, bool *ok, QString title, long default_value, long max_value)
{
    SpeedLimitDialog dlg(parent);
    dlg.setWindowTitle(title);
    dlg.setupDialog(max_value / 1024., default_value / 1024.);
    if (dlg.exec() == QDialog::Accepted) {
        *ok = true;
        int val = dlg.getSpeedLimit();
        if (val <= 0)
            return 0;
        return val * 1024;
    }
    else {
        *ok = false;
        return -2;
    }
}

void SpeedLimitDialog::updateSpinValue(int val) const
{
    qDebug("Called updateSpinValue with %d", val);
    if (val <= 0) {
        m_ui->spinBandwidth->setValue(0);
        m_ui->spinBandwidth->setSpecialValueText(QString::fromUtf8(C_INFINITY));
        m_ui->spinBandwidth->setSuffix("");
    }
    else {
        m_ui->spinBandwidth->setValue(val);
        m_ui->spinBandwidth->setSuffix(" " + tr("KiB/s"));
    }
}

void SpeedLimitDialog::updateSliderValue(int val) const
{
    if (val <= 0) {
        m_ui->spinBandwidth->setValue(0);
        m_ui->spinBandwidth->setSpecialValueText(QString::fromUtf8(C_INFINITY));
        m_ui->spinBandwidth->setSuffix("");
    }
    if (val > m_ui->bandwidthSlider->maximum())
        m_ui->bandwidthSlider->setMaximum(val);
    m_ui->bandwidthSlider->setValue(val);
}

long SpeedLimitDialog::getSpeedLimit() const
{
    long val = m_ui->bandwidthSlider->value();
    if (val > 0)
        return val;
    return -1;
}

void SpeedLimitDialog::setupDialog(long max_slider, long val) const
{
    if (val < 0)
        val = 0;
    if (max_slider <= 0)
        max_slider = 10000;
    // This can happen for example if global rate limit is lower
    // than torrent rate limit.
    if (val > max_slider)
        max_slider = val;
    m_ui->bandwidthSlider->setMaximum(max_slider);
    m_ui->bandwidthSlider->setValue(val);
    updateSpinValue(val);
}
