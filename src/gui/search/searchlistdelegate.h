#ifndef SEARCHLISTDELEGATE_H
#define SEARCHLISTDELEGATE_H

#include <QItemDelegate>

class SearchListDelegate: public QItemDelegate
{
public:
    explicit SearchListDelegate(QObject *parent);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QWidget *createEditor(QWidget *, const QStyleOptionViewItem &, const QModelIndex &) const override;
};

#endif
