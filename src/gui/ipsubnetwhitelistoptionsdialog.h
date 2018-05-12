#ifndef OPTIONS_IPSUBNETWHITELIST_H
#define OPTIONS_IPSUBNETWHITELIST_H

#include <QDialog>

class QSortFilterProxyModel;
class QStringListModel;

namespace Ui
{
    class IPSubnetWhitelistOptionsDialog;
}

class IPSubnetWhitelistOptionsDialog : public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY(IPSubnetWhitelistOptionsDialog)

public:
    explicit IPSubnetWhitelistOptionsDialog(QWidget *parent = nullptr);
    ~IPSubnetWhitelistOptionsDialog();

private slots:
    void on_buttonBox_accepted();
    void on_buttonWhitelistIPSubnet_clicked();
    void on_buttonDeleteIPSubnet_clicked();
    void on_txtIPSubnet_textChanged(const QString &subnetStr);

private:
    Ui::IPSubnetWhitelistOptionsDialog *m_ui;
    QStringListModel *m_model;
    QSortFilterProxyModel *m_sortFilter;
    bool m_modified;
};

#endif // OPTIONS_IPSUBNETWHITELIST_H
