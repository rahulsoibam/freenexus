#include "cookiesdialog.h"

#include <algorithm>

#include "base/net/downloadmanager.h"
#include "base/settingsstorage.h"
#include "cookiesmodel.h"
#include "guiiconprovider.h"
#include "ui_cookiesdialog.h"
#include "utils.h"

#define SETTINGS_KEY(name) "CookiesDialog/" name
const QString KEY_SIZE = SETTINGS_KEY("Size");
const QString KEY_COOKIESVIEWSTATE = SETTINGS_KEY("CookiesViewState");

CookiesDialog::CookiesDialog(QWidget *parent)
    : QDialog(parent)
    , m_ui(new Ui::CookiesDialog)
    , m_cookiesModel(new CookiesModel(Net::DownloadManager::instance()->allCookies(), this))
{
    m_ui->setupUi(this);

    setWindowIcon(GuiIconProvider::instance()->getIcon("preferences-web-browser-cookies"));
    m_ui->buttonAdd->setIcon(GuiIconProvider::instance()->getIcon("list-add"));
    m_ui->buttonDelete->setIcon(GuiIconProvider::instance()->getIcon("list-remove"));
    m_ui->buttonAdd->setIconSize(Utils::Gui::mediumIconSize());
    m_ui->buttonDelete->setIconSize(Utils::Gui::mediumIconSize());

    m_ui->treeView->setModel(m_cookiesModel);
    if (m_cookiesModel->rowCount() > 0)
        m_ui->treeView->selectionModel()->setCurrentIndex(
                    m_cookiesModel->index(0, 0),
                    QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);

    Utils::Gui::resize(this, SettingsStorage::instance()->loadValue(KEY_SIZE).toSize());
    m_ui->treeView->header()->restoreState(
                SettingsStorage::instance()->loadValue(KEY_COOKIESVIEWSTATE).toByteArray());
}

CookiesDialog::~CookiesDialog()
{
    SettingsStorage::instance()->storeValue(KEY_SIZE, size());
    SettingsStorage::instance()->storeValue(
                KEY_COOKIESVIEWSTATE, m_ui->treeView->header()->saveState());
    delete m_ui;
}

void CookiesDialog::accept()
{
    Net::DownloadManager::instance()->setAllCookies(m_cookiesModel->cookies());
    QDialog::accept();
}

void CookiesDialog::onButtonAddClicked()
{
    int row = m_ui->treeView->selectionModel()->currentIndex().row() + 1;

    m_cookiesModel->insertRow(row);
    m_ui->treeView->selectionModel()->setCurrentIndex(
                m_cookiesModel->index(row, 0), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
}

void CookiesDialog::onButtonDeleteClicked()
{
    QModelIndexList idxs = m_ui->treeView->selectionModel()->selectedRows();

    // sort in descending order
    std::sort(idxs.begin(), idxs.end(),
        [](const QModelIndex &l, const QModelIndex &r)
        {
            return (l.row() > r.row());
        }
    );

    for (const QModelIndex &idx : idxs)
        m_cookiesModel->removeRow(idx.row());
}
