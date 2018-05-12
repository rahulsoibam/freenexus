#ifndef BANDWIDTH_ALLOCATION_H
#define BANDWIDTH_ALLOCATION_H

#include <QDialog>
#include <QList>

#include "base/bittorrent/session.h"
#include "base/utils/misc.h"

namespace Ui
{
    class bandwidth_dlg;
}

class SpeedLimitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SpeedLimitDialog(QWidget *parent);
    ~SpeedLimitDialog();
    static long askSpeedLimit(QWidget *parent, bool *ok, QString title, long default_value, long max_value=10240000);

protected slots:
    void updateSpinValue(int val) const;
    void updateSliderValue(int val) const;
    void setupDialog(long max_slider, long val) const;

private:
    long getSpeedLimit() const;

    Ui::bandwidth_dlg *m_ui;
};

#endif // BANDWIDTH_ALLOCATION_H
