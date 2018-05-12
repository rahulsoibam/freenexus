#ifndef PROPLISTDELEGATE_H
#define PROPLISTDELEGATE_H

#include <QItemDelegate>

class QAbstractItemModel;
class QModelIndex;
class QPainter;
class QStyleOptionViewItem;
class PropertiesWidget;

// Defines for properties list columns
enum PropColumn
{
    NAME,
    PCSIZE,
    PROGRESS,
    PRIORITY,
    REMAINING,
    AVAILABILITY
};

class PropListDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    PropListDelegate(PropertiesWidget *properties);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem & /* option */, const QModelIndex &index) const override;

public slots:
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex & /* index */) const override;

signals:
    void filteredFilesChanged() const;

private:
    PropertiesWidget *m_properties;
};

#endif // PROPLISTDELEGATE_H
