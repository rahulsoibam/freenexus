#ifndef PEERLISTSORTMODEL_H
#define PEERLISTSORTMODEL_H

#include <QSortFilterProxyModel>
#include <QStringList>

#include "peerlistdelegate.h"

class PeerListSortModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    PeerListSortModel(QObject *parent = nullptr)
        : QSortFilterProxyModel(parent)
    {
    }

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const
    {
        switch (sortColumn()) {
        case PeerListDelegate::IP:
        case PeerListDelegate::CLIENT: {
                const QString strL = left.data().toString();
                const QString strR = right.data().toString();
                const int result = Utils::String::naturalCompare(strL, strR, Qt::CaseInsensitive);
                return (result < 0);
        }
            break;
        default:
            return QSortFilterProxyModel::lessThan(left, right);
        };
    }
};

#endif // PEERLISTSORTMODEL_H
