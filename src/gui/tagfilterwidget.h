#ifndef TAGFILTERWIDGET_H
#define TAGFILTERWIDGET_H

#include <QTreeView>

class TagFilterWidget: public QTreeView
{
    Q_OBJECT

public:
    explicit TagFilterWidget(QWidget *parent = nullptr);

    QString currentTag() const;

signals:
    void tagChanged(const QString &tag);
    void actionResumeTorrentsTriggered();
    void actionPauseTorrentsTriggered();
    void actionDeleteTorrentsTriggered();

private slots:
    void onCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous);
    void showMenu(QPoint);
    void callUpdateGeometry();
    void addTag();
    void removeTag();
    void removeUnusedTags();

private:
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
    void rowsInserted(const QModelIndex &parent, int start, int end) override;
    QString askTagName();
};

#endif // TAGFILTERWIDGET_H
