#include "ipsubnetwhitelistoptionsdialog.h"

#include <QHostAddress>
#include <QMessageBox>
#include <QPair>
#include <QSortFilterProxyModel>
#include <QStringListModel>

#include "base/global.h"
#include "base/preferences.h"
#include "base/utils/net.h"
#include "ui_ipsubnetwhitelistoptionsdialog.h"
#include "utils.h"

IPSubnetWhitelistOptionsDialog::IPSubnetWhitelistOptionsDialog(QWidget *parent)
    : QDialog(parent)
    , m_ui(new Ui::IPSubnetWhitelistOptionsDialog)
    , m_modified(false)
{
    m_ui->setupUi(this);

    QStringList authSubnetWhitelistStringList;
    for (const Utils::Net::Subnet &subnet : copyAsConst(Preferences::instance()->getWebUiAuthSubnetWhitelist()))
        authSubnetWhitelistStringList << Utils::Net::subnetToString(subnet);
    m_model = new QStringListModel(authSubnetWhitelistStringList, this);

    m_sortFilter = new QSortFilterProxyModel(this);
    m_sortFilter->setDynamicSortFilter(true);
    m_sortFilter->setSourceModel(m_model);

    m_ui->whitelistedIPSubnetList->setModel(m_sortFilter);
    m_ui->whitelistedIPSubnetList->sortByColumn(0, Qt::AscendingOrder);
    m_ui->buttonWhitelistIPSubnet->setEnabled(false);

    Utils::Gui::resize(this);
}

IPSubnetWhitelistOptionsDialog::~IPSubnetWhitelistOptionsDialog()
{
    delete m_ui;
}

void IPSubnetWhitelistOptionsDialog::on_buttonBox_accepted()
{
    if (m_modified) {
        // save to session
        QStringList subnets;
        // Operate on the m_sortFilter to grab the strings in sorted order
        for (int i = 0; i < m_sortFilter->rowCount(); ++i)
            subnets.append(m_sortFilter->index(i, 0).data().toString());
        Preferences::instance()->setWebUiAuthSubnetWhitelist(subnets);
        QDialog::accept();
    }
    else {
        QDialog::reject();
    }
}

void IPSubnetWhitelistOptionsDialog::on_buttonWhitelistIPSubnet_clicked()
{
    bool ok = false;
    const Utils::Net::Subnet subnet = Utils::Net::parseSubnet(m_ui->txtIPSubnet->text(), &ok);
    if (!ok) {
        QMessageBox::critical(this, tr("Error"), tr("The entered subnet is invalid."));
        return;
    }

    m_model->insertRow(m_model->rowCount());
    m_model->setData(m_model->index(m_model->rowCount() - 1, 0), Utils::Net::subnetToString(subnet));
    m_ui->txtIPSubnet->clear();
    m_modified = true;
}

void IPSubnetWhitelistOptionsDialog::on_buttonDeleteIPSubnet_clicked()
{
    for (const auto &i : copyAsConst(m_ui->whitelistedIPSubnetList->selectionModel()->selectedIndexes()))
        m_sortFilter->removeRow(i.row());

    m_modified = true;
}

void IPSubnetWhitelistOptionsDialog::on_txtIPSubnet_textChanged(const QString &subnetStr)
{
    m_ui->buttonWhitelistIPSubnet->setEnabled(Utils::Net::canParseSubnet(subnetStr));
}
