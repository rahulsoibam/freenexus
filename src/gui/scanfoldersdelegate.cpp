#include <QDebug>
#include <QPainter>
#include <QComboBox>
#include <QFileDialog>
#include <QTreeView>
#include <QItemSelectionModel>

#include "base/scanfoldersmodel.h"
#include "base/bittorrent/session.h"
#include "scanfoldersdelegate.h"


ScanFoldersDelegate::ScanFoldersDelegate(QObject *parent, QTreeView *foldersView)
    : QItemDelegate(parent)
    , m_folderView(foldersView)
{
}

void ScanFoldersDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *combobox = static_cast<QComboBox*>(editor);
    // Set combobox index
    if (index.data(Qt::UserRole).toInt() == ScanFoldersModel::CUSTOM_LOCATION)
        combobox->setCurrentIndex(4); // '4' is the index of the item after the separator in the QComboBox menu
    else
        combobox->setCurrentIndex(index.data(Qt::UserRole).toInt());
}

QWidget *ScanFoldersDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &index) const
{
    if (index.column() != ScanFoldersModel::DOWNLOAD) return 0;

    QComboBox* editor = new QComboBox(parent);

    editor->setFocusPolicy(Qt::StrongFocus);
    editor->addItem(ScanFoldersModel::pathTypeDisplayName(ScanFoldersModel::DOWNLOAD_IN_WATCH_FOLDER));
    editor->addItem(ScanFoldersModel::pathTypeDisplayName(ScanFoldersModel::DEFAULT_LOCATION));
    editor->addItem(ScanFoldersModel::pathTypeDisplayName(ScanFoldersModel::CUSTOM_LOCATION));
    if (index.data(Qt::UserRole).toInt() == ScanFoldersModel::CUSTOM_LOCATION) {
        editor->insertSeparator(3);
        editor->addItem(index.data().toString());
    }

    connect(editor, SIGNAL(currentIndexChanged(int)), this, SLOT(comboboxIndexChanged(int)));
    return editor;
}

void ScanFoldersDelegate::comboboxIndexChanged(int index)
{
    if (index == ScanFoldersModel::CUSTOM_LOCATION) {
        QWidget *w = static_cast<QWidget *>(sender());
        if (w && w->parentWidget())
            w->parentWidget()->setFocus();
    }
}

void ScanFoldersDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *combobox = static_cast<QComboBox*>(editor);
    int value = combobox->currentIndex();

    switch (value) {
    case ScanFoldersModel::DOWNLOAD_IN_WATCH_FOLDER:
    case ScanFoldersModel::DEFAULT_LOCATION:
        model->setData(index, value, Qt::UserRole);
        break;

    case ScanFoldersModel::CUSTOM_LOCATION:
        model->setData(
                    index,
                    QFileDialog::getExistingDirectory(
                        0, tr("Select save location"),
                        index.data(Qt::UserRole).toInt() == ScanFoldersModel::CUSTOM_LOCATION ?
                            index.data().toString() :
                            BitTorrent::Session::instance()->defaultSavePath()),
                    Qt::DisplayRole);
        break;

    default:
        break;
    }
}

void ScanFoldersDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const
{
    qDebug("UpdateEditor Geometry called");
    editor->setGeometry(option.rect);
}
