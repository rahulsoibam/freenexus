#include "tagfilterwidget.h"

#include <QAction>
#include <QDebug>
#include <QHeaderView>
#include <QLayout>
#include <QMenu>
#include <QMessageBox>

#include "base/bittorrent/session.h"
#include "autoexpandabledialog.h"
#include "guiiconprovider.h"
#include "tagfiltermodel.h"
#include "tagfilterproxymodel.h"
#include "utils.h"

namespace
{
    QString getTagFilter(const TagFilterProxyModel *const model, const QModelIndex &index)
    {
        QString tagFilter; // Defaults to All
        if (index.isValid()) {
            if (index.row() == 1)
                tagFilter = "";  // Untagged
            else if (index.row() > 1)
                tagFilter = model->tag(index);
        }
        return tagFilter;
    }
}

TagFilterWidget::TagFilterWidget(QWidget *parent)
    : QTreeView(parent)
{
    TagFilterProxyModel *proxyModel = new TagFilterProxyModel(this);
    proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setSourceModel(new TagFilterModel(this));
    setModel(proxyModel);
    setFrameShape(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setUniformRowHeights(true);
    setHeaderHidden(true);
    setIconSize(Utils::Gui::smallIconSize());
#if defined(Q_OS_MAC)
    setAttribute(Qt::WA_MacShowFocusRect, false);
#endif
    setIndentation(0);
    setContextMenuPolicy(Qt::CustomContextMenu);
    sortByColumn(0, Qt::AscendingOrder);
    setCurrentIndex(model()->index(0, 0));

    connect(this, &TagFilterWidget::collapsed, this, &TagFilterWidget::callUpdateGeometry);
    connect(this, &TagFilterWidget::expanded, this, &TagFilterWidget::callUpdateGeometry);
    connect(this, &TagFilterWidget::customContextMenuRequested, this, &TagFilterWidget::showMenu);
    connect(selectionModel(), &QItemSelectionModel::currentRowChanged, this
            , &TagFilterWidget::onCurrentRowChanged);
    connect(model(), &QAbstractItemModel::modelReset, this, &TagFilterWidget::callUpdateGeometry);
}

QString TagFilterWidget::currentTag() const
{
    QModelIndex current;
    auto selectedRows = selectionModel()->selectedRows();
    if (!selectedRows.isEmpty())
        current = selectedRows.first();

    return getTagFilter(static_cast<TagFilterProxyModel *>(model()), current);
}

void TagFilterWidget::onCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);

    emit tagChanged(getTagFilter(static_cast<TagFilterProxyModel *>(model()), current));
}

void TagFilterWidget::showMenu(QPoint)
{
    QMenu menu(this);

    QAction *addAct = menu.addAction(
        GuiIconProvider::instance()->getIcon("list-add")
        , tr("Add tag..."));
    connect(addAct, &QAction::triggered, this, &TagFilterWidget::addTag);

    auto selectedRows = selectionModel()->selectedRows();
    if (!selectedRows.empty() && !TagFilterModel::isSpecialItem(selectedRows.first())) {
        QAction *removeAct = menu.addAction(
            GuiIconProvider::instance()->getIcon("list-remove")
            , tr("Remove tag"));
        connect(removeAct, &QAction::triggered, this, &TagFilterWidget::removeTag);
    }

    QAction *removeUnusedAct = menu.addAction(
        GuiIconProvider::instance()->getIcon("list-remove")
        , tr("Remove unused tags"));
    connect(removeUnusedAct, &QAction::triggered, this, &TagFilterWidget::removeUnusedTags);

    menu.addSeparator();

    QAction *startAct = menu.addAction(
        GuiIconProvider::instance()->getIcon("media-playback-start")
        , tr("Resume torrents"));
    connect(startAct, &QAction::triggered
        , this, &TagFilterWidget::actionResumeTorrentsTriggered);

    QAction *pauseAct = menu.addAction(
        GuiIconProvider::instance()->getIcon("media-playback-pause")
        , tr("Pause torrents"));
    connect(pauseAct, &QAction::triggered, this
        , &TagFilterWidget::actionPauseTorrentsTriggered);

    QAction *deleteTorrentsAct = menu.addAction(
        GuiIconProvider::instance()->getIcon("edit-delete")
        , tr("Delete torrents"));
    connect(deleteTorrentsAct, &QAction::triggered, this
        , &TagFilterWidget::actionDeleteTorrentsTriggered);

    menu.exec(QCursor::pos());
}

void TagFilterWidget::callUpdateGeometry()
{
    updateGeometry();
}

QSize TagFilterWidget::sizeHint() const
{
    return {
        // Width should be exactly the width of the content
        sizeHintForColumn(0),
        // Height should be exactly the height of the content
        static_cast<int>(sizeHintForRow(0) * (model()->rowCount() + 0.5)),
    };
}

QSize TagFilterWidget::minimumSizeHint() const
{
    QSize size = sizeHint();
    size.setWidth(6);
    return size;
}

void TagFilterWidget::rowsInserted(const QModelIndex &parent, int start, int end)
{
    QTreeView::rowsInserted(parent, start, end);
    updateGeometry();
}

QString TagFilterWidget::askTagName()
{
    bool ok = false;
    QString tag = "";
    bool invalid = true;
    while (invalid) {
        invalid = false;
        tag = AutoExpandableDialog::getText(
            this, tr("New Tag"), tr("Tag:"), QLineEdit::Normal, tag, &ok).trimmed();
        if (ok && !tag.isEmpty()) {
            if (!BitTorrent::Session::isValidTag(tag)) {
                QMessageBox::warning(
                    this, tr("Invalid tag name")
                    , tr("Tag name '%1' is invalid").arg(tag));
                invalid = true;
            }
        }
    }

    return ok ? tag : QString();
}

void TagFilterWidget::addTag()
{
    const QString tag = askTagName();
    if (tag.isEmpty()) return;

    if (BitTorrent::Session::instance()->tags().contains(tag))
        QMessageBox::warning(this, tr("Tag exists"), tr("Tag name already exists."));
    else
        BitTorrent::Session::instance()->addTag(tag);
}

void TagFilterWidget::removeTag()
{
    auto selectedRows = selectionModel()->selectedRows();
    if (!selectedRows.empty() && !TagFilterModel::isSpecialItem(selectedRows.first())) {
        BitTorrent::Session::instance()->removeTag(
            static_cast<TagFilterProxyModel *>(model())->tag(selectedRows.first()));
        updateGeometry();
    }
}

void TagFilterWidget::removeUnusedTags()
{
    auto session = BitTorrent::Session::instance();
    foreach (const QString &tag, session->tags())
        if (model()->data(static_cast<TagFilterProxyModel *>(model())->index(tag), Qt::UserRole) == 0)
            session->removeTag(tag);
    updateGeometry();
}
