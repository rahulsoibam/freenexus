#include <QCoreApplication>
#include <QModelIndex>
#include <QPainter>
#include <QProgressBar>
#include <QStyleOptionViewItem>

#include "base/utils/misc.h"
#include "searchlistdelegate.h"
#include "searchsortmodel.h"

namespace
{
    const char i18nContext[] = "SearchListDelegate";
}

SearchListDelegate::SearchListDelegate(QObject *parent)
    : QItemDelegate(parent)
{
}

void SearchListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();

    QStyleOptionViewItem opt = QItemDelegate::setOptions(index, option);
    QItemDelegate::drawBackground(painter, opt, index);

    switch (index.column()) {
    case SearchSortModel::SIZE:
        opt.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
        QItemDelegate::drawDisplay(painter, opt, option.rect, Utils::Misc::friendlyUnit(index.data().toLongLong()));
        break;
    case SearchSortModel::SEEDS:
    case SearchSortModel::LEECHES:
        opt.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
        QItemDelegate::drawDisplay(painter, opt, option.rect
            , (index.data().toLongLong() >= 0) ? index.data().toString() : QCoreApplication::translate(i18nContext, "Unknown"));
        break;
    default:
        QItemDelegate::paint(painter, option, index);
    }

    painter->restore();
}

QWidget *SearchListDelegate::createEditor(QWidget *, const QStyleOptionViewItem &, const QModelIndex &) const
{
    // No editor here
    return nullptr;
}
