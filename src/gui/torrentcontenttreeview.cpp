#include "torrentcontenttreeview.h"

#include <QHeaderView>
#include <QKeyEvent>
#include <QModelIndexList>
#include <QTableView>

#include "torrentcontentmodelitem.h"

TorrentContentTreeView::TorrentContentTreeView(QWidget *parent)
    : QTreeView(parent)
{
    // This hack fixes reordering of first column with Qt5.
    // https://github.com/qtproject/qtbase/commit/e0fc088c0c8bc61dbcaf5928b24986cd61a22777
    QTableView unused;
    unused.setVerticalHeader(header());
    header()->setParent(this);
    header()->setStretchLastSection(false);
    unused.setVerticalHeader(new QHeaderView(Qt::Horizontal));
}

void TorrentContentTreeView::keyPressEvent(QKeyEvent *event)
{
    if ((event->key() != Qt::Key_Space) && (event->key() != Qt::Key_Select)) {
        QTreeView::keyPressEvent(event);
        return;
    }

    event->accept();

    QModelIndex current = currentNameCell();

    QVariant value = current.data(Qt::CheckStateRole);
    if (!value.isValid()) {
        Q_ASSERT(false);
        return;
    }

    Qt::CheckState state = (static_cast<Qt::CheckState>(value.toInt()) == Qt::Checked
                            ? Qt::Unchecked : Qt::Checked);

    const QModelIndexList selection = selectionModel()->selectedRows(TorrentContentModelItem::COL_NAME);

    for (QModelIndexList::const_iterator i = selection.begin(); i != selection.end(); ++i) {
        QModelIndex index = *i;
        Q_ASSERT(i->column() == TorrentContentModelItem::COL_NAME);
        model()->setData(index, state, Qt::CheckStateRole);
    }
}

QModelIndex TorrentContentTreeView::currentNameCell()
{
    QModelIndex current = currentIndex();
    if (!current.isValid()) {
        Q_ASSERT(false);
        return QModelIndex();
    }

    return model()->index(current.row(), TorrentContentModelItem::COL_NAME, current.parent());
}
