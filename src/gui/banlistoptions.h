#ifndef OPTIONS_BANLIST_H
#define OPTIONS_BANLIST_H

#include <QDialog>

class QSortFilterProxyModel;
class QStringListModel;

namespace Ui
{
    class BanListOptions;
}

class BanListOptions : public QDialog
{
    Q_OBJECT

public:
    explicit BanListOptions(QWidget *parent = nullptr);
    ~BanListOptions();

private slots:
    void on_buttonBox_accepted();
    void on_buttonBanIP_clicked();
    void on_buttonDeleteIP_clicked();
    void on_txtIP_textChanged(const QString &ip);

private:
    Ui::BanListOptions *m_ui;
    QStringListModel *m_model;
    QSortFilterProxyModel *m_sortFilter;
    bool m_modified;
};

#endif // OPTIONS_BANLIST_H
