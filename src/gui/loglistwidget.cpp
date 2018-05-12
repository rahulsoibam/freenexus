#include <QKeyEvent>
#include <QApplication>
#include <QClipboard>
#include <QListWidgetItem>
#include <QLabel>
#include <QRegExp>
#include <QAction>
#include "loglistwidget.h"
#include "guiiconprovider.h"

LogListWidget::LogListWidget(int maxLines, const Log::MsgTypes &types, QWidget *parent)
    : QListWidget(parent)
    , m_maxLines(maxLines)
    , m_types(types)
{
    // Allow multiple selections
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    // Context menu
    QAction *copyAct = new QAction(GuiIconProvider::instance()->getIcon("edit-copy"), tr("Copy"), this);
    QAction *clearAct = new QAction(GuiIconProvider::instance()->getIcon("edit-clear"), tr("Clear"), this);
    connect(copyAct, SIGNAL(triggered()), SLOT(copySelection()));
    connect(clearAct, SIGNAL(triggered()), SLOT(clear()));
    addAction(copyAct);
    addAction(clearAct);
    setContextMenuPolicy(Qt::ActionsContextMenu);
}

void LogListWidget::showMsgTypes(const Log::MsgTypes &types)
{
    m_types = types;
    for (int i = 0; i < count(); ++i) {
        QListWidgetItem *tempItem = item(i);
        if (!tempItem) continue;

        Log::MsgType itemType = static_cast<Log::MsgType>(tempItem->data(Qt::UserRole).toInt());
        setRowHidden(i, !(m_types & itemType));
    }
}

void LogListWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->matches(QKeySequence::Copy))
        copySelection();
    else if (event->matches(QKeySequence::SelectAll))
        selectAll();
}

void LogListWidget::appendLine(const QString &line, const Log::MsgType &type)
{
    QListWidgetItem *item = new QListWidgetItem;
    // We need to use QLabel here to support rich text
    QLabel *lbl = new QLabel(line);
    lbl->setContentsMargins(4, 2, 4, 2);
    item->setSizeHint(lbl->sizeHint());
    item->setData(Qt::UserRole, type);
    insertItem(0, item);
    setItemWidget(item, lbl);
    setRowHidden(0, !(m_types & type));

    const int nbLines = count();
    // Limit log size
    if (nbLines > m_maxLines)
        delete takeItem(nbLines - 1);
}

void LogListWidget::copySelection()
{
    static QRegExp htmlTag("<[^>]+>");
    QStringList strings;
    foreach (QListWidgetItem* it, selectedItems())
        strings << static_cast<QLabel*>(itemWidget(it))->text().replace(htmlTag, "");

    QApplication::clipboard()->setText(strings.join("\n"));
}
