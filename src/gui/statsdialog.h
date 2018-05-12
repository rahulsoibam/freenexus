#ifndef STATSDIALOG_H
#define STATSDIALOG_H

#include <QDialog>

namespace Ui
{
    class StatsDialog;
}

class StatsDialog: public QDialog
{
    Q_OBJECT

public:
  explicit StatsDialog(QWidget *parent);
  ~StatsDialog() override;

private slots:
    void update();

private:
    Ui::StatsDialog *m_ui;
};

#endif // STATSDIALOG_H
