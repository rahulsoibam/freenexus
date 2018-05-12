#ifndef TORRENTCONTENTTREEVIEW_H
#define TORRENTCONTENTTREEVIEW_H

#include <QTreeView>

class TorrentContentTreeView: public QTreeView
{
    Q_OBJECT

public:
    explicit TorrentContentTreeView(QWidget *parent = 0);
    void keyPressEvent(QKeyEvent *event);

private:
    QModelIndex currentNameCell();
};

#endif // TORRENTCONTENTTREEVIEW_H
