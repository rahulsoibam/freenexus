#ifndef UPDOWNRATIODLG_H
#define UPDOWNRATIODLG_H

#include <QDialog>

namespace Ui
{
    class UpDownRatioDlg;
}

class UpDownRatioDlg : public QDialog
{
    Q_OBJECT

public:
    UpDownRatioDlg(bool useDefault, qreal initialValue, qreal maxValue,
            int initialTimeValue, int maxTimeValue,
            QWidget *parent = nullptr);
    ~UpDownRatioDlg();

    bool useDefault() const;
    qreal ratio() const;
    int seedingTime() const;

public slots:
    void accept();

private slots:
    void handleRatioTypeChanged();
    void enableRatioSpin();
    void enableTimeSpin();

private:
    Ui::UpDownRatioDlg *m_ui;
};

#endif // UPDOWNRATIODLG_H
