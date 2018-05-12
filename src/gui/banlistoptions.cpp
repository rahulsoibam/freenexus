#include "banlistoptions.h"

#include <QHostAddress>
#include <QMessageBox>
#include <QSortFilterProxyModel>
#include <QStringListModel>

#include "base/bittorrent/session.h"
#include "base/utils/net.h"
#include "ui_banlistoptions.h"
#include "utils.h"

BanListOptions::BanListOptions(QWidget *parent)
    : QDialog(parent)
    , m_ui(new Ui::BanListOptions)
    , m_modified(false)
{
    m_ui->setupUi(this);
    m_model = new QStringListModel(BitTorrent::Session::instance()->bannedIPs(), this);

    m_sortFilter = new QSortFilterProxyModel(this);
    m_sortFilter->setDynamicSortFilter(true);
    m_sortFilter->setSourceModel(m_model);

    m_ui->bannedIPList->setModel(m_sortFilter);
    m_ui->bannedIPList->sortByColumn(0, Qt::AscendingOrder);
    m_ui->buttonBanIP->setEnabled(false);

    Utils::Gui::resize(this);
}

BanListOptions::~BanListOptions()
{
    delete m_ui;
}

void BanListOptions::on_buttonBox_accepted()
{
    if (m_modified) {
        // save to session
        QStringList IPList;
        // Operate on the m_sortFilter to grab the strings in sorted order
        for (int i = 0; i < m_sortFilter->rowCount(); ++i) {
            QModelIndex index = m_sortFilter->index(i, 0);
            IPList << index.data().toString();
        }
        BitTorrent::Session::instance()->setBannedIPs(IPList);
        QDialog::accept();
    }
    else {
        QDialog::reject();
    }
}

void BanListOptions::on_buttonBanIP_clicked()
{
    QString ip = m_ui->txtIP->text();
    if (!Utils::Net::isValidIP(ip)) {
        QMessageBox::warning(this, tr("Warning"), tr("The entered IP address is invalid."));
        return;
    }
    // the same IPv6 addresses could be written in different forms;
    // QHostAddress::toString() result format follows RFC5952;
    // thus we avoid duplicate entries pointing to the same address
    ip = QHostAddress(ip).toString();
    for (int i = 0; i < m_sortFilter->rowCount(); ++i) {
        QModelIndex index = m_sortFilter->index(i, 0);
        if (ip == index.data().toString()) {
            QMessageBox::warning(this, tr("Warning"), tr("The entered IP is already banned."));
            return;
        }
    }

    m_model->insertRow(m_model->rowCount());
    m_model->setData(m_model->index(m_model->rowCount() - 1, 0), ip);
    m_ui->txtIP->clear();
    m_modified = true;
}

void BanListOptions::on_buttonDeleteIP_clicked()
{
    QModelIndexList selection = m_ui->bannedIPList->selectionModel()->selectedIndexes();
    for (auto &i : selection)
        m_sortFilter->removeRow(i.row());

    m_modified = true;
}

void BanListOptions::on_txtIP_textChanged(const QString &ip)
{
    m_ui->buttonBanIP->setEnabled(Utils::Net::isValidIP(ip));
}
