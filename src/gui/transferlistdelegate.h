#ifndef TRANSFERLISTDELEGATE_H
#define TRANSFERLISTDELEGATE_H

#include <QItemDelegate>

QT_BEGIN_NAMESPACE
class QPainter;
class QStyleOptionViewItem;
class QModelIndex;
QT_END_NAMESPACE

namespace BitTorrent
{
    enum class TorrentState;
}
// Defines for download list list columns

class TransferListDelegate: public QItemDelegate
{
    Q_OBJECT

public:
    TransferListDelegate(QObject *parent);
    void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
    QWidget* createEditor(QWidget*, const QStyleOptionViewItem &, const QModelIndex &) const;
    QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const;

private:
    QString getStatusString(const BitTorrent::TorrentState state) const;
};

#endif // TRANSFERLISTDELEGATE_H
