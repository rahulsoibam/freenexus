#include "torrentcategorydialog.h"

#include <QMap>
#include <QMessageBox>

#include "base/bittorrent/session.h"
#include "ui_torrentcategorydialog.h"

TorrentCategoryDialog::TorrentCategoryDialog(QWidget *parent)
    : QDialog {parent}
    , m_ui {new Ui::TorrentCategoryDialog}
{
    m_ui->setupUi(this);
    m_ui->comboSavePath->setMode(FileSystemPathEdit::Mode::DirectorySave);
    m_ui->comboSavePath->setDialogCaption(tr("Choose save path"));
}

TorrentCategoryDialog::~TorrentCategoryDialog()
{
    delete m_ui;
}

QString TorrentCategoryDialog::createCategory(QWidget *parent, const QString &parentCategoryName)
{
    using BitTorrent::Session;

    QString newCategoryName {parentCategoryName};
    if (!newCategoryName.isEmpty())
        newCategoryName += QLatin1Char('/');
    newCategoryName += tr("New Category");

    TorrentCategoryDialog dialog(parent);
    dialog.setCategoryName(newCategoryName);
    while (dialog.exec() == TorrentCategoryDialog::Accepted) {
        newCategoryName = dialog.categoryName();

        if (!BitTorrent::Session::isValidCategoryName(newCategoryName)) {
            QMessageBox::critical(
                        parent, tr("Invalid category name")
                        , tr("Category name cannot contain '\\'.\n"
                             "Category name cannot start/end with '/'.\n"
                             "Category name cannot contain '//' sequence."));
        }
        else if (BitTorrent::Session::instance()->categories().contains(newCategoryName)) {
            QMessageBox::critical(
                        parent, tr("Category creation error")
                        , tr("Category with the given name already exists.\n"
                             "Please choose a different name and try again."));
        }
        else {
            Session::instance()->addCategory(newCategoryName, dialog.savePath());
            return newCategoryName;
        }
    }

    return {};
}

void TorrentCategoryDialog::editCategory(QWidget *parent, const QString &categoryName)
{
    using BitTorrent::Session;

    Q_ASSERT(Session::instance()->categories().contains(categoryName));

    TorrentCategoryDialog dialog(parent);
    dialog.setCategoryNameEditable(false);
    dialog.setCategoryName(categoryName);
    dialog.setSavePath(Session::instance()->categories()[categoryName]);
    if (dialog.exec() == TorrentCategoryDialog::Accepted) {
        Session::instance()->editCategory(categoryName, dialog.savePath());
    }
}

void TorrentCategoryDialog::setCategoryNameEditable(bool editable)
{
    m_ui->textCategoryName->setEnabled(editable);
}

QString TorrentCategoryDialog::categoryName() const
{
    return m_ui->textCategoryName->text();
}

void TorrentCategoryDialog::setCategoryName(const QString &categoryName)
{
    m_ui->textCategoryName->setText(categoryName);
}

QString TorrentCategoryDialog::savePath() const
{
    return m_ui->comboSavePath->selectedPath();
}

void TorrentCategoryDialog::setSavePath(const QString &savePath)
{
    m_ui->comboSavePath->setSelectedPath(savePath);
}
