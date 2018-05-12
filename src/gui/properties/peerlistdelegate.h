#ifndef PEERLISTDELEGATE_H
#define PEERLISTDELEGATE_H

#include <QItemDelegate>
#include <QPainter>

#include "base/preferences.h"
#include "base/utils/misc.h"
#include "base/utils/string.h"

class PeerListDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    enum PeerListColumns
    {
        COUNTRY,
        IP,
        PORT,
        CONNECTION,
        FLAGS,
        CLIENT,
        PROGRESS,
        DOWN_SPEED,
        UP_SPEED,
        TOT_DOWN,
        TOT_UP,
        RELEVANCE,
        DOWNLOADING_PIECE,
        IP_HIDDEN,

        COL_COUNT
    };

    PeerListDelegate(QObject *parent) : QItemDelegate(parent) {}

    ~PeerListDelegate() {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        painter->save();

        const bool hideValues = Preferences::instance()->getHideZeroValues();
        QStyleOptionViewItem opt = QItemDelegate::setOptions(index, option);
        QItemDelegate::drawBackground(painter, opt, index);

        switch (index.column()) {
        case PORT:
            opt.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
            QItemDelegate::drawDisplay(painter, opt, option.rect, index.data().toString());
            break;
        case TOT_DOWN:
        case TOT_UP: {
                qlonglong size = index.data().toLongLong();
                if (hideValues && (size <= 0))
                    break;
                opt.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
                QItemDelegate::drawDisplay(painter, opt, option.rect, Utils::Misc::friendlyUnit(size));
            }
            break;
        case DOWN_SPEED:
        case UP_SPEED: {
                qreal speed = index.data().toDouble();
                if (speed <= 0.0)
                    break;
                opt.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
                QItemDelegate::drawDisplay(painter, opt, opt.rect, Utils::Misc::friendlyUnit(speed, true));
            }
            break;
        case PROGRESS:
        case RELEVANCE: {
                qreal progress = index.data().toDouble();
                opt.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
                QItemDelegate::drawDisplay(painter, opt, opt.rect, Utils::String::fromDouble(progress * 100.0, 1) + "%");
            }
            break;
        default:
            QItemDelegate::paint(painter, option, index);
        }

        painter->restore();
    }

    QWidget *createEditor(QWidget *, const QStyleOptionViewItem &, const QModelIndex &) const override
    {
        // No editor here
        return nullptr;
    }
};

#endif // PEERLISTDELEGATE_H
