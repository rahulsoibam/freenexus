#include "pluginsourcedlg.h"

#include "ui_pluginsourcedlg.h"
#include "utils.h"

PluginSourceDlg::PluginSourceDlg(QWidget *parent)
    : QDialog(parent)
    , m_ui(new Ui::PluginSourceDlg())
{
    m_ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    Utils::Gui::resize(this);
    show();
}

PluginSourceDlg::~PluginSourceDlg()
{
    delete m_ui;
}

void PluginSourceDlg::on_localButton_clicked()
{
    emit askForLocalFile();
    close();
}

void PluginSourceDlg::on_urlButton_clicked()
{
    emit askForUrl();
    close();
}
