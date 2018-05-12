#ifndef PREVIEWLISTDELEGATE_H
#define PREVIEWLISTDELEGATE_H

#include <QApplication>
#include <QItemDelegate>
#include <QModelIndex>
#include <QPainter>
#ifdef Q_OS_WIN
#include <QProxyStyle>
#endif
#include <QStyleOptionProgressBar>
#include <QStyleOptionViewItem>

#include "base/utils/misc.h"
#include "base/utils/string.h"
#include "previewselectdialog.h"

class PreviewListDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    PreviewListDelegate(QObject *parent = nullptr)
        : QItemDelegate(parent)
    {
    }

    ~PreviewListDelegate() {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        painter->save();
        QStyleOptionViewItem opt = QItemDelegate::setOptions(index, option);

        switch (index.column()) {
        case PreviewSelectDialog::SIZE:
            QItemDelegate::drawBackground(painter, opt, index);
            QItemDelegate::drawDisplay(painter, opt, option.rect, Utils::Misc::friendlyUnit(index.data().toLongLong()));
            break;
        case PreviewSelectDialog::PROGRESS: {
                QStyleOptionProgressBar newopt;
                qreal progress = index.data().toDouble() * 100.;
                newopt.rect = opt.rect;
                newopt.text = ((progress == 100.0) ? QString("100%") : Utils::String::fromDouble(progress, 1) + "%");
                newopt.progress = static_cast<int>(progress);
                newopt.maximum = 100;
                newopt.minimum = 0;
                newopt.state |= QStyle::State_Enabled;
                newopt.textVisible = true;
#ifndef Q_OS_WIN
                QApplication::style()->drawControl(QStyle::CE_ProgressBar, &newopt, painter);
#else
                // XXX: To avoid having the progress text on the right of the bar
                QProxyStyle st("fusion");
                st.drawControl(QStyle::CE_ProgressBar, &newopt, painter, 0);
#endif
            }
            break;
        default:
            QItemDelegate::paint(painter, option, index);
        }

        painter->restore();
    }

    QWidget *createEditor(QWidget *, const QStyleOptionViewItem &, const QModelIndex &) const
    {
        // No editor here
        return nullptr;
    }
};

#endif // PREVIEWLISTDELEGATE_H
