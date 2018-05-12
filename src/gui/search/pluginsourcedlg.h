#ifndef PLUGINSOURCEDLG_H
#define PLUGINSOURCEDLG_H

#include <QDialog>

namespace Ui
{
    class PluginSourceDlg;
}

class PluginSourceDlg: public QDialog
{
    Q_OBJECT

public:
    explicit PluginSourceDlg(QWidget *parent = 0);
    ~PluginSourceDlg();

signals:
    void askForUrl();
    void askForLocalFile();

private slots:
    void on_localButton_clicked();
    void on_urlButton_clicked();

private:
    Ui::PluginSourceDlg *m_ui;
};

#endif // PLUGINSOURCEDLG_H
