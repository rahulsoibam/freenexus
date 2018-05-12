#ifndef SCANFOLDERSDELEGATE_H
#define SCANFOLDERSDELEGATE_H

#include <QItemDelegate>

class QPainter;
class QModelIndex;
class QStyleOptionViewItem;
class QAbstractItemModel;
class PropertiesWidget;
class QTreeView;

class ScanFoldersDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    ScanFoldersDelegate(QObject *parent, QTreeView *foldersView);

private slots:
    void comboboxIndexChanged(int index);

private:
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &index) const;

    QTreeView *m_folderView;
};

#endif // SCANFOLDERSDELEGATE_H

